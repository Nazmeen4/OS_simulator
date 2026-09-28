#include "ScisSos.h"

static void start_simulation(int number_of_processes)
{
    printf("\n============================================================\n");
    printf("                    OS SIMULATION\n");
    printf("============================================================\n");
    printf("Processes to create : %d\n", number_of_processes);

#if defined(SCHEDFUNC) && SCHEDFUNC == sched_fifo
    printf("Scheduling algorithm : FIFO / FCFS\n");
#elif defined(SCHEDFUNC) && SCHEDFUNC == sched_rr
    printf("Scheduling algorithm : Round Robin\n");
#elif defined(SCHEDFUNC) && SCHEDFUNC == sched_priority
    printf("Scheduling algorithm : Priority\n");
#endif

    ScisSosProcess *creator = ScisSosPCreator(number_of_processes);
    if (creator == NULL)
    {
        printf("ERROR: Could not create process creator.\n");
        return;
    }

    scissos_call_scheduler();

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