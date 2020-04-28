#include "common.h"

struct sh_struct *shared_struct;
int sem_id;


int main(int argc, char **argv){
    if(argc<3){
        printf("worker1 - Wrong numer of arguments\n");
    }

    int x=0,i, z,j;
    struct tm * timeinfo;
    struct timeval tim;
    time_t t;
    sscanf(argv[2], "%d", &sem_id);
    shared_struct=sh_memory(argv[1]);


    while(1){
        if(shared_struct->made1!=0 && shared_struct->made1<MAX_PRODUCTS)
            sem_op(sem_id,0,-1);

        if(shared_struct->made1>=MAX_PRODUCTS){
            sem_op(sem_id,0,1);
            disconnect_memory(shared_struct);
            raise(SIGINT);
            exit(0);
        }
        srand((unsigned) time(&t));
        i=rand()%100;
        
        j=0;
        while(j<ARRAY_SIZE){

            if(shared_struct->made1>=MAX_PRODUCTS){
                    sem_op(sem_id,0,1);
                    disconnect_memory(shared_struct);
                    exit(0);
                }
            if(shared_struct->array12[j].no==-1){
                shared_struct->array12[j].no=i;
                shared_struct->array12[j].worker=1;   
                
                sem_op(sem_id,1,1);             
                break;
            }

            j++;
            if(j==ARRAY_SIZE)
                j=0;
        }

        timeinfo=get_time();
        gettimeofday(&tim, NULL);
        x=sem_ctl(sem_id,2,GETVAL,0);
        z=sem_ctl(sem_id,1,GETVAL,0);

        
        

        printf("(%d, %d:%d:%d:%ld ) Dodalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
                (int)getpid(),timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tim.tv_usec/1000,i,z,x);
        shared_struct->made1++;
        sem_op(sem_id,0,1);
    }
    

    disconnect_memory(shared_struct);
    exit(0);
}