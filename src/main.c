#include "ScisSos.h"

void run_simulation(void);

int main(int argc, char *argv[])
{
    printf("\n============================================================\n");
    printf("             ADVANCED OPERATING SYSTEM SIMULATOR\n");
    printf("============================================================\n");

    (void)argc;
    (void)argv;

    scissos_initialise();
    run_simulation();

    return 0;
}