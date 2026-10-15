#include "ScisSos.h"

static void create_initial_processes(int count)
{
    int sizes[] = {24000, 30000, 36000, 42000, 48000};
    int priorities[] = {30, 10, 50, 20, 40, 15, 35, 25, 45, 5};
    int types[] = {PT_IOE, PT_REG, PT_CMP};

    for (int i = 0; i < count; i++)
    {
        char pname[MAXPROCNAME];
        sprintf(pname, "InitProc_%d", i + 1);

        ScisSosProcess *p = scissos_proc_create(pname, sizes[i % 5], priorities[i % 10], types[i % 3]);
        if (p != NULL)
            scissos_print_pcb(p, stdout);
    }
}

static void start_simulation(int number_of_processes)
{
    printf("\n============================================================\n");
    printf("                    OS SIMULATION\n");
    printf("============================================================\n");
    printf("Initial processes   : %d\n", INITIAL_PROCESSES);
    printf("Created by PID 0    : %d\n", number_of_processes);

#if SCHED_ALGO == 1
    printf("Scheduling algorithm : FIFO / FCFS\n");
#elif SCHED_ALGO == 2
    printf("Scheduling algorithm : Round Robin\n");
#else
    printf("Scheduling algorithm : Priority\n");
#endif

    create_initial_processes(INITIAL_PROCESSES);

    ScisSosProcess *creator = ScisSosPCreator(number_of_processes);
    if (creator == NULL)
    {
        printf("ERROR: Could not create process creator.\n");
        return;
    }

    scissos_call_scheduler();

    _proctable[0] = NULL;
    free(creator->_pcb);
    free(creator);

    printf("\n[SIMULATION COMPLETE]\n");
}

void run_simulation(void)
{
    int number_of_processes = TOTAL_PROCESSES;
    start_simulation(number_of_processes);
    scissos_print_timings();
}