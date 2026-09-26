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
int gettimeofday(struct timeval *tp, void *tzp) {
    (void)tzp; FILETIME ft; unsigned __int64 tmpres = 0;
    if (tp != NULL) {
        GetSystemTimeAsFileTime(&ft);
        tmpres |= ft.dwHighDateTime; tmpres <<= 32; tmpres |= ft.dwLowDateTime;
        tmpres /= 10; tmpres -= 11644473600000000ULL;
        tp->tv_sec = (long)(tmpres / 1000000UL); tp->tv_usec = (long)(tmpres % 1000000UL);
    }
    return 0;
}
#endif

/* Fixed to return by value to prevent static buffer overwrites during nested calls */
struct timeval _diff_times(struct timeval t1, struct timeval t2) {
    struct timeval diff;
    long sec = t1.tv_sec - t2.tv_sec;
    long usec = t1.tv_usec - t2.tv_usec;
    if (usec < 0) {
        sec--;
        usec += 1000000;
    }
    diff.tv_sec = sec;
    diff.tv_usec = usec;
    return diff;
}

struct timeval _add_times(struct timeval t1, struct timeval t2) {
    struct timeval sum;
    long sec = t1.tv_sec + t2.tv_sec;
    long usec = t1.tv_usec + t2.tv_usec;
    if (usec >= 1000000) {
        sec++;
        usec -= 1000000;
    }
    sum.tv_sec = sec;
    sum.tv_usec = usec;
    return sum;
}

struct timeval _max_times(struct timeval t1, struct timeval t2) {
    if ((t1.tv_sec > t2.tv_sec) || (t1.tv_sec == t2.tv_sec && t1.tv_usec > t2.tv_usec)) {
        return t1;
    }
    return t2;
}

void scissos_initialise(void) {
    gettimeofday(&_basetime, NULL);
    ready_head = 0; 
    ready_tail = 0; 
    ready_count = 0;

    for (int i = 0; i < MAXPROC; i++) {
        _proctable[i] = NULL;
        _readyQ[i] = EMPTY;
        _blockQ[i] = EMPTY;

        pr_times[i].crt_time.tv_sec = 0; pr_times[i].crt_time.tv_usec = 0;
        pr_times[i].rsp_time.tv_sec = 0; pr_times[i].rsp_time.tv_usec = 0;
        pr_times[i].wt_time.tv_sec = 0;  pr_times[i].wt_time.tv_usec = 0;
        pr_times[i].run_time.tv_sec = 0; pr_times[i].run_time.tv_usec = 0;
    }

    /* Initialize Special Creator Process (PID 0) */
    ScisSosPCB *p0_pcb = (ScisSosPCB *)malloc(sizeof(ScisSosPCB));
    p0_pcb->pid = 0;
    p0_pcb->uid = 0;
    p0_pcb->size = 0;
    p0_pcb->priority_value = 0;
    p0_pcb->ps_state = PS_RDY;
    p0_pcb->p_type = PT_REG;
    p0_pcb->pc = 0;
    p0_pcb->p_timeslice = DEFTS;
    p0_pcb->p_code = NULL;
    _proctable[0] = p0_pcb;
    
    enqueue_ready(0);
}

void enqueue_ready(int pid) {
    if (ready_count < MAXPROC) {
        _readyQ[ready_tail] = pid;
        ready_tail = (ready_tail + 1) % MAXPROC;
        ready_count++;
    }
}

int dequeue_ready(void) {
    if (ready_count > 0) {
        int pid = _readyQ[ready_head];
        _readyQ[ready_head] = EMPTY;
        ready_head = (ready_head + 1) % MAXPROC;
        ready_count--;
        return pid;
    }
    return EMPTY;
}

int sched_fifo(void) { return dequeue_ready(); }
int sched_rr(void) { return dequeue_ready(); }

void scissos_print_timings(int total_processes) {
    printf("\n=================================================================================================\n");
    printf("                                  PROCESS TIMING METRICS REPORT                                  \n");
    printf("=================================================================================================\n");
    printf("%-6s | %-15s | %-15s | %-15s | %-15s\n", 
           "PID", "Creation (s.us)", "Response (s.us)", "Run Time (s.us)", "Wait Time (s.us)");
    printf("-------------------------------------------------------------------------------------------------\n");

    for (int i = 1; i <= total_processes; i++) {
        if (_proctable[i] != NULL) {
            printf("P%-5d | %ld.%06ld      | %ld.%06ld      | %ld.%06ld      | %ld.%06ld\n",
                   i,
                   (long)pr_times[i].crt_time.tv_sec, (long)pr_times[i].crt_time.tv_usec,
                   (long)pr_times[i].rsp_time.tv_sec, (long)pr_times[i].rsp_time.tv_usec,
                   (long)pr_times[i].run_time.tv_sec, (long)pr_times[i].run_time.tv_usec,
                   (long)pr_times[i].wt_time.tv_sec,  (long)pr_times[i].wt_time.tv_usec);
        }
    }
    printf("=================================================================================================\n\n");
}