#ifndef SCISSOS_H
#define SCISSOS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// ============================================================================
// 1. SYSTEM CONFIGURATION & CONSTANTS (#define)
// ============================================================================
#define MAXPROC              100     // Maximum process table capacity
#define DEFAULT_PROCESS_SIZE 24000   // Instructions per process
#define DEFAULT_TIME_SLICE   50      // Instructions per CPU burst quantum
#define SPECIAL_CREATOR_PID  0       // Dedicated PID for dynamic process creator
#define IDLE_PID            -1       // CPU idle state flag

// System Call Probabilities (Percentage threshold for Long System Calls)
#define PROB_COMP_LONG       0.05    // 5% long calls for Computation-Bound
#define PROB_REGULAR_LONG    0.20    // 20% long calls for Regular
#define PROB_IO_LONG         0.50    // 50% long calls for I/O-Bound

// ============================================================================
// 2. CATEGORICAL ENUMERATIONS (enum)
// ============================================================================

// Workload Classification
typedef enum {
    REGULAR = 0,            // Balanced CPU / IO execution
    COMPUTATION_BOUND,      // CPU heavy workload
    IO_BOUND                // Frequent IO / Long syscall workload
} ProcessType;

// Process Lifecycle States
typedef enum {
    STATE_NEW,              // Process instantiated
    STATE_READY,            // Enqueued in Ready Queue
    STATE_RUN,              // Currently executing on CPU
    STATE_BLOCKED,          // Enqueued in Blocked Queue
    STATE_TERMINATED        // Execution completed
} ProcessState;

// System Call Identifiers
typedef enum {
    SYSCALL_SHORT = 0,      // Fast syscall ('0')
    SYSCALL_LONG  = 1       // Slow/Blocking syscall ('1')
} SyscallType;

// Scheduling Algorithm Selector
typedef enum {
    SCHED_FIFO,             // First-In First-Out (FCFS)
    SCHED_ROUND_ROBIN,      // Round Robin
    SCHED_PRIORITY          // Priority-based
} SchedAlgorithm;

// ============================================================================
// 3. CORE OS DATA STRUCTURES
// ============================================================================

// Instruction Tuple
typedef struct {
    SyscallType syscall_type; // SYSCALL_SHORT or SYSCALL_LONG
    unsigned int mem_addr;    // Virtual address space reference
} ProcessInstruction;

// Process Control Block (PCB)
typedef struct {
    int pid;                   // Process ID
    int uid;                   // User ID
    int size;                  // Total instruction count
    int priority;              // Process priority level
    ProcessState process_state;// STATE_READY, STATE_RUN, STATE_BLOCKED, etc.
    int pc;                    // Program Counter (index into instruction array)
    ProcessType process_type;  // REGULAR, COMPUTATION_BOUND, IO_BOUND
    int memory_behaviour;      // Memory access pattern identifier
    int time_slice;            // Assigned execution quantum
    
    void *pointer_to_code;     // Generic code memory pointer
    void *page_table;          // Page table structure reference
    
    // Performance & Timing Metrics
    time_t creation_time;      // Timestamp when process was instantiated
    time_t start_time;         // Timestamp of first CPU schedule
    time_t completion_time;    // Timestamp when process terminated
    time_t last_ready_time;    // Timestamp when entering READY/BLOCKED queue
    
    double response_time;      // Elapsed time to first execution (start - creation)
    double waiting_time;       // Accumulated time spent in READY/BLOCKED queues
    double running_time;       // Accumulated CPU execution time
} PCB;

// Main Process Object
typedef struct {
    int pid;                       // Metadata Process ID
    int uid;                       // Owner User ID
    int size;                      // Total instructions count
    PCB pcb;                       // Embedded Process Control Block
    ProcessInstruction *instructions; // Dynamic array of instructions
} Process;

// Queue Data Structure (Array-based FIFO implementation)
typedef struct {
    int data[MAXPROC];
    int front;
    int rear;
    int count;
} Queue;

// ============================================================================
// 4. GLOBAL OS STATE DECLARATIONS (extern)
// ============================================================================
extern Process *process_table[MAXPROC]; // Global Process Table
extern Queue ready_queue;               // Global Ready Queue
extern Queue blocked_queue;             // Global Blocked Queue
extern int total_processes_created;     // Total spawned process counter

// ============================================================================
// 5. FUNCTION PROTOTYPES BY MODULE
// ============================================================================

// --- Queue Operations (simulator.c) ---
void init_queue(Queue *q);
int is_queue_empty(Queue *q);
int is_queue_full(Queue *q);
void enqueue(Queue *q, int pid);
int dequeue(Queue *q);

// --- Process Management (process.c) ---
Process* create_process(int pid, int uid, int size);
void free_process(Process *proc);
void run_process(int pid, int time_slice);
void run_special_creator_process(int total_to_create);

// --- Simulator Core & Scheduler (scheduler.c / simulator.c) ---
void init_os(void);
void schedule_and_dispatch(SchedAlgorithm algo);
void print_performance_metrics(void);

#endif // SCISSOS_H