#include "ScisSos.h"


/*
 * Implemented in simulator.c
 */
void run_simulation(void);


int main(int argc, char *argv[])
{
    /*
     * The scheduling algorithm is selected at
     * compile time through SCHEDFUNC in ScisSos.h.
     *
     * Therefore this main simply initialises
     * and runs the simulator.
     */


    printf(
        "\n============================================================\n"
    );

    printf(
        "             ADVANCED OPERATING SYSTEM SIMULATOR\n"
    );

    printf(
        "============================================================\n"
    );


    /*
     * Avoid compiler warning if argc/argv are unused.
     */

    (void)argc;
    (void)argv;


    /*
     * Initialise operating system.
     */

    scissos_initialise();


    /*
     * Start simulation.
     */

    run_simulation();


    return 0;
}