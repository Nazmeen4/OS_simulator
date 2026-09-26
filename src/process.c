\#include "ScisSos.h"

\#include \<sys/time.h>

ScisSosProcess \*scissos_proc_create(char \* *name*, int *size*, int *priority*, int *pid*){

    ScisSosProcess \*p = (ScisSosProcess \*)malloc(sizeof(ScisSosProcess));

    if(p ==NULL){return NULL;}

    p->\_PID = pid;

    p->\_psize = size;

    strcpy(p->\_pname, name);

    ScisSosPCB \*pcb = (ScisSosPCB \*)malloc(sizeof(ScisSosPCB));

    if(pcb==NULL){return NULL;}

    p->\_pcb = pcb;

    pcb->pid = pid;

    pcb->uid = 1;

    pcb->size = size;

    pcb->state=PS_RDY;

    proctable[pid] = pcb;

    int ty=rand()%3;;

    int long_p;

    if(ty==0){

        pcb->p_type=PT_REG;

        long_p=REG_THR;

    }else if(ty==1){

        pcb->p_type=PT_CMP;

        long_p=CMP_THR;

    }else{

        pcb->p_type=PT_IOE;

        long_p=IOE_THR;

    }

    p->\_CODE = (ScisSosInst \*\*)malloc(sizeof(ScisSosInst \*)\*size);

    if(p->\_CODE==NULL){return NULL;}

    for(int i=0;i\<size;i++){

        p->\_CODE[i] = (ScisSosInst \*)malloc(sizeof(ScisSosInst));

        if(p->\_CODE[i]==NULL){return NULL;}

        p->\_CODE[i]->\_inum = i;

        p->\_CODE[i]->\_addref = 100;

        if(rand()/RAND_MAX\<long_p){

            p->\_CODE[i]->\_syscall = INS_LNG;

        }else{

            p->\_CODE[i]->\_syscall = INS_SHR;

        }

    }

    gettimeofday(&pr_times[pid].crt_time, NULL);

    return p;

}

int scissos_proc_run(int *pid*){

    if(pid<1 || pid>MAXPROC){return -1;}

    ScisSosPCB \*pcb=proctable[pid]->\_pcb;

    if(pcb==NULL){return -1;}

    if(pcb->state!=PS_RDY){return -1;}

    pcb->state=PS_RUN;

    if(pcb->pc==0){gettimeofday(&pr_times[pid].rsp_time, NULL);}

    struct timeval t;

    gettimeofday(&t, NULL);

    return 0;

}