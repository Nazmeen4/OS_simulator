#ifndef SCISSOS_H
#define SCISSOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
int gettimeofday(struct timeval *tp, void *tzp);
#else
#include <sys/time.h>
#endif

#define MAXPROC     1000                /* Max number of processes */
#define MAXUSRS     10                  /* Max number of users */
#define DEFPRIO     20                  /* Default priority for process */
#define EMPTY       -100                /* Unfilled entries */
#define MAXPGES     10                  /* Max number of pages/process */
#define DEFTS       30                /* Default time slice */
#define REG_THR     0.02                /* Normal process: 2% long calls */
#define CMP_THR     0.001               /* Compute Intensive: 0.1% */
#define IOE_THR     0.2                 /* IO Intensive: 20% long calls */
#define MAXPROCNAME 80                  /* Max Process Name Length */

/**** Constants for Process States ****/
#define PS_NEW      0
#define PS_RDY      1
#define PS_RUN      2
#define PS_BLK      3
#define PS_SRDY     4
#define PS_SBLK     5
#define PS_DEAD     6

/**** Constants for Process Types ****/
#define PT_REG      0                   
#define PT_CMP      1                   
#define PT_IOE      2                   
#define INS_LNG     10                  
#define INS_SHR     20                  

typedef int ScisSosPGTable[2];

typedef struct {
     int _inum;
     int _syscall;
     int _addref;
} ScisSosInst;

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

typedef struct {
     char _pname[MAXPROCNAME];
     int _PID;
     int _psize;
     ScisSosPCB *_pcb;
     ScisSosInst **_CODE;
} ScisSosProcess;

/** Timing structure using struct timeval **/
typedef struct {
     struct timeval crt_time;
     struct timeval rsp_time;
     struct timeval wt_time;
     struct timeval run_time;
     struct timeval comp_time;
} TIMING;

/* Global Variables */
extern ScisSosPCB *_proctable[MAXPROC];
extern int _readyQ[MAXPROC];
extern int _blockQ[MAXPROC];
extern struct timeval _basetime;
extern TIMING pr_times[MAXPROC];

/* Functions */
void scissos_initialise(void);
void enqueue_ready(int pid);
int dequeue_ready(void);
int sched_fifo(void);
int sched_rr(void);
void scissos_print_timings(int total_processes);

struct timeval _diff_times(struct timeval t1, struct timeval t2);
struct timeval _add_times(struct timeval t1, struct timeval t2);
struct timeval _max_times(struct timeval t1, struct timeval t2);

ScisSosProcess *scissos_proc_create(char *pname, int size, int prio, int p_type);
int scissos_proc_run(int pid);
void run_simulation(int algo_choice, int target_process_count);

#endif