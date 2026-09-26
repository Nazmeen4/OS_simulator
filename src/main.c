#include "ScisSos.h"

int main(void) {
    scissos_initialise();

    int algo_choice = 1; /* 1 = FIFO, 2 = Round Robin */
    int total_processes_to_generate = 4;

    printf("\nStarting OS Execution Simulation:\n");
    printf(" - Special PID 0: Kernel / Creator\n");
    printf(" - Total User Processes: %d\n", total_processes_to_generate);
    printf(" - Scheduling Mode: %s\n", (algo_choice == 1) ? "FIFO" : "Round Robin");

    run_simulation(algo_choice, total_processes_to_generate);

    return 0;
}