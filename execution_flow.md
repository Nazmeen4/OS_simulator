# Advanced Operating System Simulator (`ScisSos`) - Execution Flow

This document outlines the complete execution flow of your OS simulator, broken down step-by-step from system startup to the final timing metrics report.

---

### Step 1: System Initialization (`main.c` & `scheduler.c`)
1. **Program Entry (`main`)**: Execution starts in `main.c`, which prints the simulator banner and calls `scissos_initialise()`.
2. **OS Base Setup**: `scissos_initialise()` records the exact starting wall-clock time (`_basetime`) using `gettimeofday()`. It resets all process tables (`_proctable`), ready queues (`_readyQ`), block queues, and timing statistics matrices (`pr_times`), then seeds the random number generator (`srand`).

### Step 2: Simulation Setup & Creator Process (`simulator.c` & `process.c`)
1. **Simulation Start (`run_simulation`)**: Sets the target process count to **20** (`TOTAL_PROCESSES`) and invokes `start_simulation()`.
2. **PID 0 Creation (`ScisSosPCreator`)**: A special system process with **PID 0** ("PROCESS_CREATOR") is initialized. Its job is to spawn all user processes. It is immediately marked as ready and pushed to the ready queue.

### Step 3: The Scheduler Loop (`scheduler.c`)
1. **Scheduler Start (`scissos_call_scheduler`)**: An infinite `while` loop runs as long as there are active or remaining processes in the system.
2. **Process Selection (`SCHEDFUNC`)**: The scheduler calls the active scheduling function defined in `ScisSos.h` (e.g., `sched_rr` for Round Robin, `sched_fifo`, or `sched_priority`) via `dequeue_ready()`:
   * **FIFO**: Selects the first process and assigns a large time slice (`FIFO_TS = 24000`) so it runs to completion.
   * **Round Robin**: Selects the first process and assigns the default time slice (`DEFTS = 2`).
   * **Priority**: Scans the ready queue for the highest priority value, extracts it, and assigns the default time slice (`DEFTS = 2`).

### Step 4: Process Execution & Lifecycle (`process.c`)
The selected process runs via `scissos_proc_run(pid)`:

* **If PID 0 (Creator) Runs**:
  * It checks if it has created all 20 processes yet.
  * If not, it generates a new user process (`UserProc_X`), assigns it a random type (`PT_IOE`, `PT_REG`, or `PT_CMP`), allocates **24,000 instructions** (`NUM_PROCESSES`), assigns a priority from a predefined array, and calls `scissos_proc_create()`.
  * The newly created user process is initialized, its creation time is logged relative to `_basetime`, and it enters the ready queue (`PS_RDY`).
  * Once all 20 processes are spawned, PID 0 changes its state to `PS_DEAD`.

* **If a User Process Runs (PID > 0)**:
  * **Response Time Calculation**: On its *very first* execution turn, it calculates its **Response Time** (time elapsed from creation to first CPU execution).
  * **Instruction Execution Loop**: The process executes instructions sequentially up to its assigned `p_timeslice` limit.
  * **System Calls**: 
    * If an instruction triggers a long system call (`INS_LNG`), a CPU interrupt fires, the process yields the CPU early, and it returns to the ready queue.
  * **Burst Timing**: Real execution duration is measured using `gettimeofday()` and added to its cumulative run time.
  * **Termination vs Re-queue**:
    * If the process finishes all instructions (`pc >= size`), its state becomes `PS_DEAD`, and its final turnaround and **waiting times** are calculated.
    * If its time slice expires or it hits a long system call before finishing, its state is reset to `PS_RDY` and it is pushed back to the back of the ready queue via `enqueue_ready()`.

### Step 5: Cleanup & Timing Metrics (`scheduler.c` & `process.c`)
1. **Process Deletion**: When any user process hits `PS_DEAD`, `scissos_proc_delete(pid)` is called to safely free all instruction arrays, code memory blocks, PCBs, and process control blocks from system memory.
2. **Simulation Complete**: Once all user processes complete and the ready queue is empty, the scheduler loop breaks.
3. **Final Report (`scissos_print_timings`)**: A formatted performance metrics table is printed to the console, listing each PID alongside its exact **Creation**, **Response**, **Run Time**, and **Wait Time**.