#include "ScisSos.h"


/*
 * Keep the complete process structure separately.
 *
 * The official process table stores PCB pointers,
 * so this array lets us later free the complete
 * ScisSosProcess as well.
 */
static ScisSosProcess *_processes[MAXPROC];


/*
 * Generate process instructions.
 *
 * PT_REG -> 2% long calls
 * PT_CMP -> 0.1% long calls
 * PT_IOE -> 20% long calls
 */
static ScisSosInst **generate_process_instructions(
    int size,
    int p_type
)
{
    ScisSosInst **code;

    double threshold;


    code = (ScisSosInst **)malloc(
        size * sizeof(ScisSosInst *)
    );

    if (code == NULL)
        return NULL;


    if (p_type == PT_CMP)
        threshold = CMP_THR;
    else if (p_type == PT_IOE)
        threshold = IOE_THR;
    else
        threshold = REG_THR;


    for (int i = 0; i < size; i++)
    {
        code[i] =
            (ScisSosInst *)malloc(
                sizeof(ScisSosInst)
            );

        if (code[i] == NULL)
        {
            for (int j = 0; j < i; j++)
                free(code[j]);

            free(code);

            return NULL;
        }


        code[i]->_inum = i;


        double r =
            (double)rand() / (double)RAND_MAX;


        if (r < threshold)
        {
            /*
             * IMPORTANT:
             * Long = 10, exactly as supplied.
             */
            code[i]->_syscall = INS_LNG;

            code[i]->_addref = 100 + i;
        }
        else
        {
            /*
             * IMPORTANT:
             * Short = 20, exactly as supplied.
             */
            code[i]->_syscall = INS_SHR;

            code[i]->_addref = 200 + i;
        }
    }


    return code;
}


/*
 * Create a user process.
 */
ScisSosProcess *scissos_proc_create(
    char *pname,
    int size,
    int prio,
    int p_type
)
{
    static int next_pid = 1;


    if (next_pid >= MAXPROC)
        return NULL;


    if (size <= 0)
        return NULL;


    ScisSosProcess *proc =
        (ScisSosProcess *)malloc(
            sizeof(ScisSosProcess)
        );

    if (proc == NULL)
        return NULL;


    ScisSosPCB *pcb =
        (ScisSosPCB *)malloc(
            sizeof(ScisSosPCB)
        );

    if (pcb == NULL)
    {
        free(proc);
        return NULL;
    }


    int pid = next_pid++;


    /*
     * Process structure.
     */

    strncpy(
        proc->_pname,
        pname,
        MAXPROCNAME - 1
    );

    proc->_pname[MAXPROCNAME - 1] = '\0';

    proc->_PID = pid;

    proc->_psize = size;


    /*
     * Generate executable code.
     */

    proc->_CODE =
        generate_process_instructions(
            size,
            p_type
        );


    if (proc->_CODE == NULL)
    {
        free(pcb);
        free(proc);

        return NULL;
    }


    /*
     * Fill PCB.
     */

    pcb->pid = pid;

    pcb->uid = 1;

    pcb->size = size;

    pcb->priority_value = prio;

    pcb->ps_state = PS_RDY;

    pcb->p_type = p_type;

    pcb->m_type = MT_GOOD;

    pcb->pc = 0;

    pcb->p_code = proc->_CODE;

    pcb->p_timeslice = DEFTS;


    /*
     * Page table is not used in Assignment 1.
     */

    for (int i = 0; i < MAXPGES; i++)
    {
        pcb->pg_table[i][0] = EMPTY;
        pcb->pg_table[i][1] = EMPTY;
    }


    proc->_pcb = pcb;


    /*
     * Put PCB into process table.
     */

    _proctable[pid] = pcb;

    _processes[pid] = proc;


    /*
     * Creation time.
     *
     * Store time relative to OS start.
     */

    struct timeval now;

    gettimeofday(&now, NULL);

    pr_times[pid].crt_time =
        *_diff_times(
            now,
            _basetime
        );


    /*
     * Initialise remaining timing values.
     */

    pr_times[pid].rsp_time.tv_sec = 0;
    pr_times[pid].rsp_time.tv_usec = 0;

    pr_times[pid].wt_time.tv_sec = 0;
    pr_times[pid].wt_time.tv_usec = 0;

    pr_times[pid].run_time.tv_sec = 0;
    pr_times[pid].run_time.tv_usec = 0;


    printf(
        "[PROCESS CREATED] PID %d | Name: %s | "
        "Priority: %d | Type: %d\n",
        pid,
        proc->_pname,
        prio,
        p_type
    );


    /*
     * New process enters READY queue.
     */

    enqueue_ready(pid);


    return proc;
}


/*
 * Create special process PID 0.
 *
 * PID 0 creates all user processes.
 */
ScisSosProcess *ScisSosPCreator(
    int number_of_processes
)
{
    ScisSosProcess *proc =
        (ScisSosProcess *)malloc(
            sizeof(ScisSosProcess)
        );

    if (proc == NULL)
        return NULL;


    ScisSosPCB *pcb =
        (ScisSosPCB *)malloc(
            sizeof(ScisSosPCB)
        );

    if (pcb == NULL)
    {
        free(proc);
        return NULL;
    }


    strcpy(
        proc->_pname,
        "PROCESS_CREATOR"
    );

    proc->_PID = 0;

    proc->_psize = number_of_processes;

    proc->_CODE = NULL;


    pcb->pid = 0;

    pcb->uid = 0;

    pcb->size = number_of_processes;

    pcb->priority_value = 0;

    pcb->ps_state = PS_RDY;

    pcb->p_type = PT_REG;

    pcb->m_type = MT_GOOD;

    pcb->pc = 0;

    pcb->p_code = NULL;

    pcb->p_timeslice = DEFTS;


    for (int i = 0; i < MAXPGES; i++)
    {
        pcb->pg_table[i][0] = EMPTY;
        pcb->pg_table[i][1] = EMPTY;
    }


    proc->_pcb = pcb;

    _processes[0] = proc;

    _proctable[0] = pcb;


    /*
     * Put creator into READY queue.
     */

    enqueue_ready(0);


    printf(
        "[CREATOR CREATED] PID 0 | "
        "Will create %d processes.\n",
        number_of_processes
    );


    return proc;
}


/*
 * Execute one scheduling turn of a process.
 *
 * Stops when:
 *
 * 1. Process finishes
 * 2. Time slice expires
 * 3. Long system call occurs
 */
int scissos_proc_run(int pid)
{
    if (pid < 0 || pid >= MAXPROC)
        return -1;


    ScisSosPCB *pcb =
        _proctable[pid];


    if (pcb == NULL)
        return -1;


    pcb->ps_state = PS_RUN;


    /*
     * Special creator process.
     */
    if (pid == 0)
    {
        if (pcb->pc < pcb->size)
        {
            char pname[MAXPROCNAME];


            sprintf(
                pname,
                "UserProc_%d",
                pcb->pc + 1
            );


            /*
             * All processes use 24000 instructions.
             */

            int process_size = 24000;


            /*
             * Generate process type.
             *
             * IO intensive: 50%
             * Regular:      30%
             * Compute:      20%
             */

            double r =
                (double)rand() / (double)RAND_MAX;


            int p_type;


            if (r < 0.50)
                p_type = PT_IOE;
            else if (r < 0.80)
                p_type = PT_REG;
            else
                p_type = PT_CMP;


            /*
             * Give different priorities so that
             * priority scheduling can demonstrate
             * a difference.
             *
             * Larger number = higher priority.
             */

            int priorities[] = {
                30,
                10,
                50,
                20,
                40,
                15,
                35,
                25,
                45,
                5
            };


            int priority =
                priorities[
                    pcb->pc % 10
                ];


            scissos_proc_create(
                pname,
                process_size,
                priority,
                p_type
            );


            pcb->pc++;


            printf(
                "[PID 0] Created process %d/%d\n",
                pcb->pc,
                pcb->size
            );
        }


        /*
         * Creator finished creating all
         * required processes.
         */

        if (pcb->pc >= pcb->size)
        {
            pcb->ps_state = PS_DEAD;

            printf(
                "[PID 0 TERMINATED] "
                "All processes created.\n"
            );
        }
        else
        {
            pcb->ps_state = PS_RDY;

            enqueue_ready(0);
        }


        return 0;
    }


    /*
     * Response time.
     *
     * Only calculate on FIRST execution.
     */

    if (pr_times[pid].rsp_time.tv_sec == 0 &&
        pr_times[pid].rsp_time.tv_usec == 0)
    {
        struct timeval now;

        gettimeofday(&now, NULL);


        struct timeval elapsed =
            *_diff_times(
                now,
                _basetime
            );


        /*
         * response =
         * first run time - creation time
         */

        struct timeval response =
            *_diff_times(
                elapsed,
                pr_times[pid].crt_time
            );


        /*
         * Guard against tiny clock anomalies.
         */

        if (response.tv_sec < 0 ||
            (response.tv_sec == 0 &&
             response.tv_usec < 0))
        {
            response.tv_sec = 0;
            response.tv_usec = 0;
        }


        pr_times[pid].rsp_time =
            response;
    }


    /*
     * Measure this CPU burst.
     */

    struct timeval start_time;

    gettimeofday(
        &start_time,
        NULL
    );


    int executed = 0;

    int long_call = 0;


    /*
     * Execute instructions.
     */

    while (pcb->pc < pcb->size &&
           executed < pcb->p_timeslice)
    {
        ScisSosInst *inst =
            pcb->p_code[pcb->pc];


        pcb->pc++;

        executed++;


        /*
         * A long system call causes the
         * process to give up CPU.
         */

        if (inst->_syscall == INS_LNG)
        {
            long_call = 1;


            printf(
                "[CPU INTERRUPT] PID %d | "
                "LONG system call at PC %d\n",
                pid,
                pcb->pc
            );


            break;
        }
    }


    /*
     * End burst timing.
     */

    struct timeval end_time;

    gettimeofday(
        &end_time,
        NULL
    );


    struct timeval burst =
        *_diff_times(
            end_time,
            start_time
        );


    /*
     * Never allow a negative burst.
     */

    if (burst.tv_sec < 0 ||
        (burst.tv_sec == 0 &&
         burst.tv_usec < 0))
    {
        burst.tv_sec = 0;
        burst.tv_usec = 0;
    }


    /*
     * Add CPU time.
     */

    pr_times[pid].run_time =
        *_add_times(
            pr_times[pid].run_time,
            burst
        );


    printf(
        "[CPU BURST] PID %d | "
        "Executed: %d | PC: %d/%d\n",
        pid,
        executed,
        pcb->pc,
        pcb->size
    );


    /*
     * Process completed.
     */

    if (pcb->pc >= pcb->size)
    {
        pcb->ps_state = PS_DEAD;


        printf(
            "[PID %d TERMINATED] "
            "All instructions completed.\n",
            pid
        );


        /*
         * Calculate final waiting time.
         *
         * turnaround =
         * completion time - creation time
         *
         * waiting =
         * turnaround - CPU running time
         */

        struct timeval now;

        gettimeofday(
            &now,
            NULL
        );


        struct timeval finish =
            *_diff_times(
                now,
                _basetime
            );


        struct timeval turnaround =
            *_diff_times(
                finish,
                pr_times[pid].crt_time
            );


        struct timeval waiting =
            *_diff_times(
                turnaround,
                pr_times[pid].run_time
            );


        /*
         * Real waiting time cannot be negative.
         *
         * The guard also protects against
         * microsecond-level clock measurement
         * anomalies.
         */

        if (waiting.tv_sec < 0 ||
            (waiting.tv_sec == 0 &&
             waiting.tv_usec < 0))
        {
            waiting.tv_sec = 0;
            waiting.tv_usec = 0;
        }


        pr_times[pid].wt_time =
            waiting;


        return 0;
    }


    /*
     * Process did not finish.
     *
     * Put it back into READY queue.
     */

    pcb->ps_state = PS_RDY;

    enqueue_ready(pid);


    if (long_call)
    {
        printf(
            "[PID %d] Returned to READY "
            "after long system call.\n",
            pid
        );
    }
    else
    {
        printf(
            "[PID %d] Time slice expired "
            "and returned to READY.\n",
            pid
        );
    }


    return 0;
}


/*
 * Delete a completed process.
 *
 * Free:
 *
 * 1. Every instruction
 * 2. Code array
 * 3. PCB
 * 4. Process structure
 */
void scissos_proc_delete(int pid)
{
    if (pid <= 0 || pid >= MAXPROC)
        return;


    ScisSosProcess *proc =
        _processes[pid];


    if (proc == NULL)
        return;


    /*
     * Free executable code.
     */

    if (proc->_CODE != NULL)
    {
        for (int i = 0;
             i < proc->_psize;
             i++)
        {
            free(proc->_CODE[i]);
        }


        free(proc->_CODE);
    }


    /*
     * Free PCB.
     */

    free(proc->_pcb);


    /*
     * Free process structure.
     */

    free(proc);


    /*
     * Remove from OS tables.
     */

    _processes[pid] = NULL;

    _proctable[pid] = NULL;
}


/*
 * Save process.
 */
int scissos_proc_save(
    ScisSosProcess *proc,
    FILE *fp
)
{
    if (proc == NULL || fp == NULL)
        return -1;


    fprintf(
        fp,
        "Process Name: %s\n",
        proc->_pname
    );

    fprintf(
        fp,
        "PID: %d\n",
        proc->_PID
    );

    fprintf(
        fp,
        "Size: %d\n",
        proc->_psize
    );


    return 0;
}


/*
 * Print PCB.
 */
void scissos_print_pcb(
    ScisSosProcess *proc,
    FILE *fp
)
{
    if (proc == NULL ||
        proc->_pcb == NULL ||
        fp == NULL)
    {
        return;
    }


    ScisSosPCB *pcb =
        proc->_pcb;


    fprintf(
        fp,
        "\n================ PCB ================\n"
    );

    fprintf(
        fp,
        "Process Name : %s\n",
        proc->_pname
    );

    fprintf(
        fp,
        "PID          : %d\n",
        pcb->pid
    );

    fprintf(
        fp,
        "UID          : %d\n",
        pcb->uid
    );

    fprintf(
        fp,
        "Size         : %d\n",
        pcb->size
    );

    fprintf(
        fp,
        "Priority     : %d\n",
        pcb->priority_value
    );

    fprintf(
        fp,
        "State        : %d\n",
        pcb->ps_state
    );

    fprintf(
        fp,
        "Process Type : %d\n",
        pcb->p_type
    );

    fprintf(
        fp,
        "Memory Type  : %d\n",
        pcb->m_type
    );

    fprintf(
        fp,
        "PC           : %d\n",
        pcb->pc
    );

    fprintf(
        fp,
        "Time Slice   : %d\n",
        pcb->p_timeslice
    );

    fprintf(
        fp,
        "=====================================\n"
    );
}