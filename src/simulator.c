#include "ScisSos.h"

static int has_active_processes(int target_process_count) {
    for (int i = 1; i <= target_process_count; i++) {
        if (_proctable[i] != NULL && _proctable[i]->ps_state != PS_DEAD) {
            return 1;
        }
    }
    return 0;
}

void run_simulation(int algo_choice, int target_process_count) {
    int total_created = 0;

    printf("\n======================================================================\n");
    printf("        STARTING EXECUTION SIMULATION (%s)                            \n", (algo_choice == 1) ? "FIFO (FCFS)" : "ROUND ROBIN");
    printf("======================================================================\n");

    while (1) {
        int current_pid = (algo_choice == 1) ? sched_fifo() : sched_rr();

        if (current_pid == EMPTY) {
            if (has_active_processes(target_process_count)) {
                continue;
            } else {
                printf("\n[CPU IDLE] Ready Queue is empty and all processes completed.\n");
                break;
            }
        }

        scissos_proc_run(current_pid);
        ScisSosPCB *pcb = _proctable[current_pid];

        /* SPECIAL CREATOR PROCESS (PID 0) */
        if (current_pid == 0) {
            printf("\n[PID 0 RUNNING] Special Process creating new user process...\n");

            if (total_created < target_process_count) {
                char pbuf[32];
                snprintf(pbuf, sizeof(pbuf), "UserProc_%d", total_created + 1);
                
                double prob = (double)rand() / (double)RAND_MAX;
                int p_type;

                if (prob < 0.50) {
                    p_type = PT_IOE;  /* 50% chance: I/O-Intensive */
                } else if (prob < 0.80) {
                    p_type = PT_REG;  /* 30% chance: Regular */
                } else {
                    p_type = PT_CMP;  /* 20% chance: Compute-Intensive */
                }
                scissos_proc_create(pbuf, 150, DEFPRIO, p_type);
                total_created++;
            }

            if (total_created < target_process_count) {
                pcb->ps_state = PS_RDY;
                enqueue_ready(0); 
            } else {
                pcb->ps_state = PS_DEAD;
                printf("[PID 0 TERMINATED] Finished creating all %d user processes.\n", target_process_count);
            }
        } 
        else {
            int quantum = (algo_choice == 1) ? pcb->size : DEFTS;
            int inst_to_run = ((pcb->pc + quantum) > pcb->size) ? (pcb->size - pcb->pc) : quantum;
            
            int executed_this_burst = 0;
            int interrupted = 0;

            struct timeval t_start, t_end;
            gettimeofday(&t_start, NULL);

            for (int i = 0; i < inst_to_run; i++) {
                if (pcb->pc < pcb->size) {
                    ScisSosInst *inst = pcb->p_code[pcb->pc];
                    pcb->pc++;
                    executed_this_burst++;

                    if (inst->_syscall == INS_LNG) {
                        printf("[CPU INTERRUPT] PID %d hit LONG SYSTEM CALL at PC %d. Yielding CPU!\n", 
                               current_pid, pcb->pc);
                        interrupted = 1;
                        break;
                    }
                }
            }

            gettimeofday(&t_end, NULL);
            struct timeval burst_duration = _diff_times(t_end, t_start);
            pr_times[current_pid].run_time = _add_times(pr_times[current_pid].run_time, burst_duration);

            printf("[CPU BURST] Executed PID %d | Ran: %d inst | PC: %d/%d\n", 
                   current_pid, executed_this_burst, pcb->pc, pcb->size);

            if (pcb->pc >= pcb->size) {
                pcb->ps_state = PS_DEAD;
                struct timeval now;
                gettimeofday(&now, NULL);
                struct timeval elapsed = _diff_times(now, _basetime);
                pr_times[current_pid].comp_time = _diff_times(elapsed, pr_times[current_pid].crt_time);
                
                struct timeval turnaround = _diff_times(pr_times[current_pid].comp_time, pr_times[current_pid].crt_time);
                pr_times[current_pid].wt_time = _diff_times(turnaround, pr_times[current_pid].run_time);

                printf("  [TERMINATED] PID %d finished all instructions.\n", current_pid);
            } 
            else {
                pcb->ps_state = PS_RDY;
                enqueue_ready(current_pid);
                if (interrupted) {
                    printf("  [RE-ENQUEUED] PID %d returned to Ready Queue due to Long System Call.\n", current_pid);
                }
            }
        }
    }

    scissos_print_timings(target_process_count);
}