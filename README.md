# Advanced Operating System Simulator (ScisSos)

Welcome to the **ScisSos** repository! This project is a modular C-based Operating System simulator designed to model process creation, state management, Process Control Block (PCB) scheduling, system call handling, and timing performance metrics.

---

## 📁 Repository Structure

Below is the project directory structure to help you understand how the code and assets are organized:

```text
OS_Simulator/
├── include/
│   └── ScisSos.h         # Global definitions, enums, PCB & Process structs, prototypes
├── src/
│   ├── process.c         # Process creation, execution, and PCB state updates
│   ├── simulator.c       # Queue management (Ready/Blocked) & OS initialization
│   ├── scheduler.c       # Scheduling algorithms (FIFO, Round Robin, etc.)
│   └── main.c            # Simulation startup, process setup, and metrics reporting
├── build/                # Directory generated for compiled output binaries
├── .gitignore            # Ignores build artifacts (.exe, .o, .obj)
├── Makefile              # Automated build script
└── README.md             # Project documentation
```

---

## 🛠️ Prerequisites & Setup

Ensure your environment meets the following requirements before building or running the project:

1. **Compiler:** Microsoft Visual C++ (`cl.exe`) included with Visual Studio or Build Tools for Visual Studio.
2. **IDE / Editor:** Visual Studio Code with the C/C++ Extension recommended.
3. **Operating System:** Windows 10/11 (uses `ws2_32.lib` for high-precision timing functions).

> 💡 **Tip:** Open your command line using the **Developer Command Prompt for Visual Studio** (or configure your VS Code terminal environment) so that `cl.exe` is recognized.

---

## 🚀 Quick Start: Build & Run

```cmd
gcc src/main.c src/process.c src/scheduler.c src/simulator.c -Iinclude -o simulator.exe -lws2_32
```
```cmd
.\simulator.exe > output.txt
```


Follow these commands to automatically create the output directory (if it doesn't exist), compile the simulator, and run it.

### 1. Build the Simulator

This command checks for the `build` directory, creates it if missing, and compiles the source files:

```cmd
if not exist build mkdir build && cl /Iinclude /Fe:build\simulator.exe src\*.c ws2_32.lib
```

### 2. Run the Simulator

Once compiled, execute the binary with:

```cmd
.\build\simulator.exe
```

---

## 🧹 Cleaning Up

To clean up all generated object files and binaries from the repository:

```cmd
del /Q build\* *.obj
```
