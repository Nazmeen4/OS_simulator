#include "ScisSos.h"

ScisSosPCB *_proctable[MAXPROC];
int _readyQ[MAXPROC];
int _blockQ[MAXPROC];
struct timeval _basetime;
TIMING pr_times[MAXPROC];

static int ready_head = 0;
static int ready_tail = 0;
static int ready_count = 0;

#if defined(_WIN32) || defined(_WIN64)
int gettimeofday(struct timeval *tp, void *tzp)
{
    (void)tzp;
    FILETIME ft;
    unsigned __int64 tmpres = 0;

    if (tp != NULL)
    {
        GetSystemTimeAsFileTime(&ft);
        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;
        tmpres /= 10;
        tmpres -= 11644473600000000ULL;
        tp->tv_sec = (long)(tmpres / 1000000UL);
        tp->tv_usec = (long)(tmpres % 1000000UL);
    }
    return 0;
}
#endif

struct timeval *_diff_times(struct timeval t1, struct timeval t2)
{
    static struct timeval result;
    result.tv_sec = t1.tv_sec - t2.tv_sec;
    result.tv_usec = t1.tv_usec - t2.tv_usec;

    if (result.tv_usec < 0)
    {
        result.tv_sec--;
        result.tv_usec += 1000000;
    }
    return &result;
}

struct timeval *_add_times(struct timeval t1, struct timeval t2)
{
    static struct timeval result;
    result.tv_sec = t1.tv_sec + t2.tv_sec;
    result.tv_usec = t1.tv_usec + t2.tv_usec;

    if (result.tv_usec >= 1000000)
    {
        result.tv_sec++;
        result.tv_usec -= 1000000;
    }
    return &result;
}

struct timeval _max_times(struct timeval t1, struct timeval t2)
{
    if (t1.tv_sec > t2.tv_sec)
        return t1;
    if (t1.tv_sec < t2.tv_sec)
        return t2;
    if (t1.tv_usec > t2.tv_usec)
        return t1;
    return t2;
}

void enqueue_ready(int pid)
{
    if (pid < 0 || pid >= MAXPROC || ready_count >= MAXPROC)
        return;

    for (int i = 0; i < ready_count; i++)
    {
        int index = (ready_head + i) % MAXPROC;
        if (_readyQ[index] == pid)
            return;
    }

    _readyQ[ready_tail] = pid;
    ready_tail = (ready_tail + 1) % MAXPROC;
    ready_count++;
}

int dequeue_ready(void)
{
    if (ready_count == 0)
        return EMPTY;

    int pid = _readyQ[ready_head];
    _readyQ[ready_head] = EMPTY;
    ready_head = (ready_head + 1) % MAXPROC;
    ready_count--;

    return pid;
}

int sched_fifo(void)
{
    int pid = dequeue_ready();
    if (pid != EMPTY && _proctable[pid] != NULL)
    {
        _proctable[pid]->p_timeslice = FIFO_TS;
    }
    return pid;
}

int sched_rr(void)
{
    int pid = dequeue_ready();
    if (pid != EMPTY && _proctable[pid] != NULL)
    {
        _proctable[pid]->p_timeslice = DEFTS;
    }
    return pid;
}

int sched_priority(void)
{
    if (ready_count == 0)
        return EMPTY;

    int selected_index = -1;
    int selected_pid = EMPTY;
    int highest_priority = -1;

    for (int i = 0; i < ready_count; i++)
    {
        int index = (ready_head + i) % MAXPROC;
        int pid = _readyQ[index];

        if (pid == EMPTY || _proctable[pid] == NULL || _proctable[pid]->ps_state != PS_RDY)
            continue;

        int priority = _proctable[pid]->priority_value;
        if (selected_index == -1 || priority > highest_priority)
        {
            selected_index = i;
            selected_pid = pid;
            highest_priority = priority;
        }
    }

    if (selected_index == -1)
        return EMPTY;

    for (int i = selected_index; i < ready_count - 1; i++)
    {
        int current = (ready_head + i) % MAXPROC;
        int next = (ready_head + i + 1) % MAXPROC;
        _readyQ[current] = _readyQ[next];
    }

    int last = (ready_head + ready_count - 1) % MAXPROC;
    _readyQ[last] = EMPTY;
    ready_count--;
    ready_tail = (ready_head + ready_count) % MAXPROC;

    if (_proctable[selected_pid] != NULL)
    {
        _proctable[selected_pid]->p_timeslice = DEFTS;
    }
    return selected_pid;
}

void scissos_initialise(void)
{
    gettimeofday(&_basetime, NULL);
    ready_head = 0;
    ready_tail = 0;
    ready_count = 0;

    for (int i = 0; i < MAXPROC; i++)
    {
        _proctable[i] = NULL;
        _readyQ[i] = EMPTY;
        _blockQ[i] = EMPTY;

        pr_times[i].crt_time.tv_sec = 0;
        pr_times[i].crt_time.tv_usec = 0;
        pr_times[i].rsp_time.tv_sec = 0;
        pr_times[i].rsp_time.tv_usec = 0;
        pr_times[i].wt_time.tv_sec = 0;
        pr_times[i].wt_time.tv_usec = 0;
        pr_times[i].run_time.tv_sec = 0;
        pr_times[i].run_time.tv_usec = 0;
    }
    srand((unsigned int)time(NULL));
    printf("[OS INITIALISED]\n");
}

static int processes_remaining(void)
{
    for (int i = 0; i < MAXPROC; i++)
    {
        if (_proctable[i] != NULL && _proctable[i]->ps_state != PS_DEAD)
            return 1;
    }
    return 0;
}

void scissos_call_scheduler(void)
{
    printf("\n============================================================\n");
    printf("                    SCHEDULER STARTED\n");
    printf("============================================================\n");

    while (1)
    {
        if (ready_count == 0)
        {
            if (!processes_remaining())
                break;
            continue;
        }

        int pid = SCHEDFUNC();
        if (pid == EMPTY || _proctable[pid] == NULL)
            continue;

        printf("\n[SCHEDULER] Selected PID %d", pid);
        printf(" | Priority: %d", _proctable[pid]->priority_value);
        printf(" | Time Slice: %d\n", _proctable[pid]->p_timeslice);

        scissos_proc_run(pid);

        if (_proctable[pid] != NULL && _proctable[pid]->ps_state == PS_DEAD)
        {
            if (pid != 0)
                scissos_proc_delete(pid);
        }
    }

    printf("\n============================================================\n");
    printf("                    SCHEDULER FINISHED\n");
    printf("============================================================\n");
}

void scissos_print_timings(void)
{
    printf("\n==========================================================================\n");
    printf("                         PROCESS TIMINGS\n");
    printf("==========================================================================\n");
    printf("%-6s | %-18s | %-18s | %-18s | %-18s\n", "PID", "Creation", "Response", "Run Time", "Wait Time");
    printf("---------------------------------------------------------------------------\n");

    for (int pid = 1; pid < MAXPROC; pid++)
    {
        if (pr_times[pid].crt_time.tv_sec == 0 && pr_times[pid].crt_time.tv_usec == 0)
            continue;

        printf("%-6d | %ld.%06ld        | %ld.%06ld        | %ld.%06ld        | %ld.%06ld\n",
               pid,
               (long)pr_times[pid].crt_time.tv_sec, (long)pr_times[pid].crt_time.tv_usec,
               (long)pr_times[pid].rsp_time.tv_sec, (long)pr_times[pid].rsp_time.tv_usec,
               (long)pr_times[pid].run_time.tv_sec, (long)pr_times[pid].run_time.tv_usec,
               (long)pr_times[pid].wt_time.tv_sec, (long)pr_times[pid].wt_time.tv_usec);
    }
    printf("==========================================================================\n");
}