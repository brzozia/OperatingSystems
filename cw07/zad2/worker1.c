#include "common.h"
sem_t *semaf[SEM_NO];
// struct sh_struct *shared_struct;


int main(int argc, char **argv){
    if(argc<2){
        printf("worker1 - Wrong numer of arguments\n");
    }

    int x=0,i, z,j;
    struct tm * timeinfo;
    struct timeval tim;
    for(int i=0;i<SEM_NO;i++){

        if(i==0)
            semaf[i]= sem_open( names[i],O_RDWR,0666,1);
        else
            semaf[i]= sem_open( names[i],O_RDWR,0666,0);
        
        if(semaf[i]==NULL){
             perror("1sem open error");
         }
    }
    char name[24];
    key_t memory_key = ftok(getenv("HOME"), 'l');
    sprintf(name, "/%d", (int)memory_key);
    int memory_id = shm_open( name, O_RDWR | O_EXCL,666);

    struct sh_struct *shared_struct=mmap( NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, memory_id,0);
    // shared_struct=sh_memory(NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, argv[1],0);

    
    while(1){
        // if(shared_struct->made1!=0)
        sem_op(0,0,-1);

        if(shared_struct->made1>=MAX_PRODUCTS){
            sem_op(0,0,1);
            disconnect_memory(shared_struct);
            exit(0);
        }

        i=rand()%100;
        
        j=0;
        while(j<ARRAY_SIZE){

            if(shared_struct->made1>=MAX_PRODUCTS){
                    sem_op(0,0,1);
                    disconnect_memory(shared_struct);
                    exit(0);
                }
            if(shared_struct->array12[j].no==-1){

                if(shared_struct->made1>=MAX_PRODUCTS){
                    sem_op(0,0,1);
                    disconnect_memory(shared_struct);
                    exit(0);
                }

                shared_struct->array12[j].no=i;
                shared_struct->array12[j].worker=1;   
                
                sem_op(0,1,1);             
                break;
            }

            j++;
            if(j==ARRAY_SIZE)
                j=0;
        }

        timeinfo=get_time();
        gettimeofday(&tim, NULL);
        x=sem_ctl(0,2);
        z=sem_ctl(0,1);

        sem_op(0,0,1);
        

        printf("(%d, %d:%d:%d:%ld ) Dodalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
                (int)getpid(),timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tim.tv_usec/1000,i,z,x);
        shared_struct->made1++;
    }
    

    disconnect_memory(shared_struct);
    exit(0);
}