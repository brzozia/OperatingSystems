
#include <stdlib.h>
#include <stdio.h>
#include<unistd.h> 
#include <sys/stat.h>
#include <stddef.h>
#include <sys/mman.h>
#include <sys/ipc.h> 
#include <string.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>


union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

#define ARRAY_SIZE 5
#define MAX_PRODUCTS 10
#define NO_OF__ONE_TYPE_WORKERS 5
#define SEM_NO 3


struct data{
    int no;
    int worker;
};

struct sh_struct{
    struct data array12[ARRAY_SIZE]; //array between first and second worker
    int sended3;
    int prepared2;
    int made1;
};

// struct semafores{
//     sem_t *semaf[SEM_NO];
// };

 sem_t *semaf[SEM_NO];
int memory_id;
// int  sh_get(key_t key, size_t size, int flag){
//     int i= shmget( key,  size,  flag);
//     if(i==-1){
//         perror("get shm");
//         exit(-1);
//     }
//     return i;
// }

struct tm * get_time(){
    time_t rawtime;
    time( &rawtime );
    return localtime( &rawtime );
}

void *sh_memory(void *addr, size_t len, int prot, int flags, char * mem, off_t offset){
    
    // int memory_id;
    // sscanf(mem,"%d",&memory_id);
    void *memory_addr=mmap(addr,  len,  prot,  flags,  memory_id,  offset);
    if(memory_addr ==(void *)-1){
        perror("worker - shared memory addr");
    }
    return memory_addr;
}

void disconnect_memory(void *addr){
    if(munmap(addr,sizeof(struct sh_struct))==-1){
        perror("worker - memory disconnect");
    }
}

void sem_get( int i, int flags){

    // struct semafores semafores_set;
    for(int i=0;i<SEM_NO;i++){
        key_t semafor_key = ftok(getenv("HOME"), 's');
        char name[24];
        sprintf(name, "/%d", (int)semafor_key);
         sem_t *add= sem_open( name,  flags,666,0);
        semaf[i]=add;
    }


    // if(sem_id==-1){
    //     perror("add semafors");
    // }
    // return semafores_set;
}


int sem_ctl( int l, int semnum){
    int res;
    int r=sem_getvalue(semaf[semnum],&res);
    if(r==-1){
        printf("ctll:%d ", semnum);
        perror("sem ctl");
    }
    return res;
}


void sem_op(int l, int sem_nr, int opr){
    if(opr>0){
        if(sem_post(semaf[sem_nr])==-1){
            perror("sem post");
        }
    }
    else{
        while(opr<0){
            if(sem_wait(semaf[sem_nr])==-1){
                perror("sem post");
            }
            opr++;
        }
    }
}