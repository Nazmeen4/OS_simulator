# Advanced Operating System Simulator (ScisSos)

A modular C-based Operating System simulator developed for the CS401 Advanced Operating Systems assignment. The simulator models process creation, state management, PCB scheduling, system call handling, and timing performance metrics.

---

## 📁 Repository Structure

```text
OS_Simulator/
├── include/
│   └── ScisSos.h         # Global definitions, enums, PCB & Process structs, prototypes
├── src/
│   ├── process.c         # Process creation, process execution, and PCB updating
│   ├── simulator.c       # Queue management (Ready/Blocked) & OS initialization
│   ├── scheduler.c       # Scheduling algorithms (FIFO, Round Robin, etc.)
│   └── main.c            # Simulation startup, special process creator, and metrics reporting
├── build/                # Directory for compiled binary output
├── .gitignore            # Ignores build artifacts (.exe, .o)
├── Makefile              # Automated build script
└── README.md             # Project documentation