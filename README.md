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
# Advanced OS Simulator in C (`ScisSos`)

A lightweight Operating System simulator built in C to demonstrate process scheduling, queue management, and system call execution flows using custom data structures.

---

## 🛠️ Prerequisites & Setup

To compile and run this simulator, ensure you have the following environment set up:

1. **Compiler:** Microsoft Visual C++ (`cl.exe`) included with Visual Studio or Build Tools for Visual Studio.
2. **IDE / Editor:** Visual Studio Code with the C/C++ Extension installed.
3. **Operating System:** Windows 10/11 (utilizes `ws2_32.lib` for high-precision timing functions).

> **Note:** Make sure you open your terminal using the **Developer Command Prompt for Visual Studio** or configure your VS Code integrated terminal to run in an MSVC environment so that `cl.exe` is recognized in your path.

---

## ⚙️ Compilation and Execution

### 1. Build the Simulator
To compile all source files, include the header directory, and link the required Windows Socket library, run:

```cmd
cl /Iinclude /Fe:build\simulator.exe src\*.c ws2_32.lib