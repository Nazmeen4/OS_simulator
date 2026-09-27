#include "ScisSos.h"


/*
 * Run one complete simulation.
 */
static void start_simulation(
    int number_of_processes
)
{
    printf(
        "\n============================================================\n"
    );

    printf(
        "                    OS SIMULATION\n"
    );

    printf(
        "============================================================\n"
    );


    printf(
        "Processes to create : %d\n",
        number_of_processes
    );


#if defined(SCHEDFUNC) && SCHEDFUNC == sched_fifo
    printf(
        "Scheduling algorithm : FIFO / FCFS\n"
    );
#endif


    /*
     * Create special creator process.
     *
     * PID 0.
     */

    ScisSosProcess *creator =
        ScisSosPCreator(
            number_of_processes
        );


    if (creator == NULL)
    {
        printf(
            "ERROR: Could not create process creator.\n"
        );

        return;
    }


    /*
     * Start scheduler.
     */

    scissos_call_scheduler();


    /*
     * PID 0 was not deleted by scheduler,
     * so free its process structure here.
     */

    free(creator->_pcb);

    free(creator);


    printf(
        "\n[SIMULATION COMPLETE]\n"
    );
}


/*
 * Public simulation function.
 */
void run_simulation(void)
{
    /*
     * Assignment requires fewer than 20 processes.
     *
     * Four gives a simple demonstration.
     */

    int number_of_processes = 4;


    start_simulation(
        number_of_processes
    );


    /*
     * Print timing results.
     */

    scissos_print_timings();
}