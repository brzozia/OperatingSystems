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


union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

#define ARRAY_SIZE 7
#define MAX_PRODUCTS 30
#define NO_OF__ONE_TYPE_WORKERS 4
#define SEM_NO 3 // one to connect with array, one to get number of made packages (packages "to prepare"), one to get number of prepared packages (packages "to send")


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



sem_t *semaf[SEM_NO];
char names[SEM_NO][24];


struct tm * get_time(){
    time_t rawtime;
    time( &rawtime );
    return localtime( &rawtime );
}


void disconnect_memory(void *addr){
    if(munmap(addr,sizeof(struct sh_struct))==-1){
        perror("memory disconnect");
    }
}



void sem_get(int flags){

    for(int i=0;i<SEM_NO;i++){

        key_t semafor_key = ftok(getenv("HOME"), i);
        char name[24];
        strcpy(name,"");
        sprintf(name, "/%d", (int)semafor_key);

        if(i==0)
            semaf[i]= sem_open( name,  flags,0666,1); 
        else
            semaf[i]= sem_open( name, flags,0666,0);
 
        if(semaf[i]==NULL){
             perror("sem open error");
         }
        strcpy(names[i],name);
    }
}


int sem_ctl(int semnum){
    int res;
    int r=sem_getvalue(semaf[semnum],&res);
    if(r==-1){
        printf("ctll:%d ", semnum);
        perror("sem ctl");
    }
    return res;
}


void sem_op( int sem_nr, int opr){
    if(opr>0){
        if(sem_post(semaf[sem_nr])==-1){
            perror("sem post");
        }
    }
    else{
        if(sem_wait(semaf[sem_nr])==-1){
            perror("sem post");
        }
        opr++;

    }
}