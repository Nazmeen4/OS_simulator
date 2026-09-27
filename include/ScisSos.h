#ifndef SCISSOS_H
#define SCISSOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>

/* MinGW provides struct timeval */
int gettimeofday(struct timeval *tp, void *tzp);

#else

#include <sys/time.h>

#endif

/****
 * Constants defining OS parameters
 ****/

#define MAXPROC     1000
#define MAXUSRS     10
#define DEFPRIO     20
#define EMPTY       -100
#define MAXPGES     10
#define DEFTS       6000
#define REG_THR     0.02
#define CMP_THR     0.001
#define IOE_THR     0.2
#define MAXPROCNAME 80

/*
 * Default scheduler.
 *
 * 1 = FIFO
 * 2 = Round Robin
 * 3 = Priority
 */
#define SCHEDFUNC   sched_fifo


/**** Constants for Process States ************************************/

#define PS_NEW  0
#define PS_RDY  1
#define PS_RUN  2
#define PS_BLK  3
#define PS_SRDY 4
#define PS_SBLK 5
#define PS_DEAD 6


/**** Constants for Process Types *************************************/

#define PT_REG  0
#define PT_CMP  1
#define PT_IOE  2

#define MT_GOOD 0
#define MT_BAD  1
#define MT_UGLY 2

#define INS_LNG 10
#define INS_SHR 20


#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#define MIN(A, B) (((A) < (B)) ? (A) : (B))


/*
 * FIFO uses a large time slice.
 *
 * Since our test processes have 24000 instructions,
 * this allows the process to normally run until completion,
 * except when it encounters a long system call.
 */
#define FIFO_TS 24000


typedef int ScisSosPGTable[2];


/** Instruction in a process **/

typedef struct {
    int _inum;
    int _syscall;
    int _addref;
} ScisSosInst;


/** Process Control Block **/

typedef struct {
    int pid;
    int uid;
    int size;
    int priority_value;
    int ps_state;
    int p_type;
    int m_type;
    int pc;
    ScisSosInst **p_code;
    ScisSosPGTable pg_table[MAXPGES];
    int p_timeslice;
} ScisSosPCB;


/** Process Structure **/

typedef struct {
    char _pname[MAXPROCNAME];
    int _PID;
    int _psize;
    ScisSosPCB *_pcb;
    ScisSosInst **_CODE;
} ScisSosProcess;


/** OS management data structures **/

extern ScisSosPCB *_proctable[MAXPROC];

extern int _readyQ[MAXPROC];

extern int _blockQ[MAXPROC];


/** Memory management constants - not used now **/

#define TBEG 0.02
#define TEND 0.02


/** Time maintenance structure **/

typedef struct {
    struct timeval crt_time;
    struct timeval rsp_time;
    struct timeval wt_time;
    struct timeval run_time;
} TIMING;


extern struct timeval _basetime;

extern TIMING pr_times[MAXPROC];


/** Timing functions **/

struct timeval *_diff_times(struct timeval, struct timeval);

struct timeval *_add_times(struct timeval, struct timeval);

struct timeval _max_times(struct timeval, struct timeval);


/** Process-related functions **/

ScisSosProcess *scissos_proc_create(
    char *,
    int,
    int,
    int
);

ScisSosProcess *ScisSosPCreator(int);

int scissos_proc_save(
    ScisSosProcess *,
    FILE *
);

void scissos_print_pcb(
    ScisSosProcess *,
    FILE *
);

int scissos_proc_run(int);

void scissos_proc_delete(int);


/** OS-related functions **/

void scissos_initialise(void);

void scissos_call_scheduler(void);

void scissos_print_timings(void);


/** Scheduling functions **/

int sched_fifo(void);

int sched_rr(void);

int sched_priority(void);


/** Ready queue functions **/

void enqueue_ready(int);

int dequeue_ready(void);


/** Memory management - NOT FOR ASSIGNMENT 1 **/

int *memory_gen_addrefstrings(int, int);


#endif