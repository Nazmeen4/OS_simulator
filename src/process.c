#include "ScisSos.h"

static ScisSosInst **generate_process_instructions(int size, int p_type) {
    ScisSosInst **code = (ScisSosInst **)malloc(size * sizeof(ScisSosInst *));
    double threshold = (p_type == PT_CMP) ? CMP_THR : (p_type == PT_IOE) ? IOE_THR : REG_THR;

    for (int i = 0; i < size; i++) {
        code[i] = (ScisSosInst *)malloc(sizeof(ScisSosInst));
        code[i]->_inum = i;
        double random_val = (double)rand() / (double)RAND_MAX;
        if (random_val < threshold) {
            code[i]->_syscall = INS_LNG;
            code[i]->_addref = 100 + i;
        } else {
            code[i]->_syscall = INS_SHR;
            code[i]->_addref = 200 + i;
        }
    }
    return code;
}

ScisSosProcess *scissos_proc_create(char *pname, int size, int prio, int p_type) {
    static int auto_pid = 1;
    int pid = auto_pid++;
    if (pid >= MAXPROC) return NULL;

    ScisSosProcess *proc = (ScisSosProcess *)malloc(sizeof(ScisSosProcess));
    proc->_PID = pid;
    proc->_psize = size;
    strncpy(proc->_pname, pname, MAXPROCNAME - 1);

    ScisSosPCB *pcb = (ScisSosPCB *)malloc(sizeof(ScisSosPCB));
    proc->_pcb = pcb;
    proc->_CODE = generate_process_instructions(size, p_type);
    
    pcb->pid = pid;
    pcb->uid = 1;
    pcb->size = size;
    pcb->priority_value = prio;
    pcb->ps_state = PS_RDY;
    pcb->p_type = p_type;
    pcb->m_type = 0;
    pcb->pc = 0;
    pcb->p_code = proc->_CODE;
    pcb->p_timeslice = DEFTS;

    _proctable[pid] = pcb;
    
    struct timeval now;
    gettimeofday(&now, NULL);
    pr_times[pid].crt_time = _diff_times(now, _basetime);
    pr_times[pid].rsp_time.tv_sec = 0; pr_times[pid].rsp_time.tv_usec = 0;
    pr_times[pid].wt_time.tv_sec = 0;  pr_times[pid].wt_time.tv_usec = 0;
    pr_times[pid].run_time.tv_sec = 0; pr_times[pid].run_time.tv_usec = 0;
    pr_times[pid].comp_time.tv_sec = 0; pr_times[pid].comp_time.tv_usec = 0;

    printf("[PROCESS CREATED] PID %d ('%s') created.\n", pid, pname);
    enqueue_ready(pid);
    return proc;
}

int scissos_proc_run(int pid) {
    if (pid < 0 || pid >= MAXPROC || _proctable[pid] == NULL) return -1;
    _proctable[pid]->ps_state = PS_RUN;

    if (pid > 0 && pr_times[pid].rsp_time.tv_sec == 0 && pr_times[pid].rsp_time.tv_usec == 0) {
        struct timeval now;
        gettimeofday(&now, NULL);
        struct timeval elapsed = _diff_times(now, _basetime);
        pr_times[pid].rsp_time = _diff_times(elapsed, pr_times[pid].crt_time);
    }
    return 0;
}