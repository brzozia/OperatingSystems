#include <stdlib.h>
#include <stdio.h>
#include<unistd.h> 
#include <sys/stat.h>
#include <stddef.h>
#include <sys/shm.h>
#include <sys/ipc.h> 
#include <string.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>


union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

#define ARRAY_SIZE 3
#define MAX_PRODUCTS 100
#define NO_OF__ONE_TYPE_WORKERS 3


struct data{
    int no;
    int worker;
};

struct sh_struct{
    struct data array12[ARRAY_SIZE]; 
    int sended3; //number of sended packages
    int prepared2; //number of all prepared packages
    int made1; //number of all made packages
    int loop1; //loops are to help with cycle array
    int loop2;
    int loop3;
};



int  sh_get(key_t key, size_t size, int flag){
    int i= shmget( key,  size,  flag);
    if(i==-1){
        perror("get shm");
        exit(-1);
    }
    return i;
}

struct tm * get_time(){
    time_t rawtime;
    time( &rawtime );
    return localtime( &rawtime );
}

void *sh_memory(char * mem){
    int memory_id;
    sscanf(mem,"%d",&memory_id);
    void *memory_addr=shmat( memory_id, NULL, 0);
    if(memory_addr ==(void *)-1){
        perror("worker - shared memory addr");
    }
    return memory_addr;
}

void disconnect_memory(void *addr){
    if(shmdt(addr)==-1){
        perror("worker - memory disconnect");
    }
}

int sem_get(key_t semafor_key, int i, int flags){
    int sem_id=semget( semafor_key, i,  flags);
    if(sem_id==-1){
        perror("add semafors");
    }
    return sem_id;
}

int sem_ctl(int semid, int semnum, int cmd, int arg){
    union semun sem_num;
    sem_num.val=arg;
    int r=semctl( semid,  semnum,  cmd,  sem_num);
    if(r==-1){
        perror("sem ctl");
    }
    return r;
}

void sem_op(int semid, int sem_nr, int opr){
    struct sembuf sem_buf;
    sem_buf.sem_num=sem_nr;
    sem_buf.sem_op=opr;
    if(semop( semid, &sem_buf,  1)==-1){
        perror("sem op");
    }
}