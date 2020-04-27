#include "common.h"

int main(int argc, char **argv){
    if(argc<3){
        printf("worker2 - Wrong numer of arguments\n");
    }

    int sem_id,p=0,x=0,j=0,z;
    sscanf(argv[2], "%d", &sem_id);
    struct sh_struct *shared_struct=sh_memory(argv[1]);
    struct tm * timeinfo;
    struct timeval tim;

    while(1){

        sem_op(sem_id,0,-1);
        if(shared_struct->prepared2>=MAX_PRODUCTS){
            // raise(SIGUSR1);
            sem_op(sem_id,0,1);
            disconnect_memory(shared_struct);
            exit(0);
        }

        for(j=0;j<ARRAY_SIZE;j++){
            if(shared_struct->array12[j].worker==1){
                shared_struct->array12[j].no*=2;
                p=shared_struct->array12[j].no;
                shared_struct->array12[j].worker=2;
                sem_op(sem_id,2,1);
                sem_op(sem_id,1,-1);
                
                timeinfo=get_time();
                gettimeofday(&tim, NULL);
                x=sem_ctl(sem_id,2,GETVAL,0);
                z=sem_ctl(sem_id,1,GETVAL,0);
                printf("(%d, %d:%d:%d:%ld ) Przygotowalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
                (int)getpid(),timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tim.tv_usec/1000,p,z,x);
                shared_struct->prepared2++;

                break;
            }
           
        }
        sem_op(sem_id,0,1);

        
    }

    disconnect_memory(shared_struct);
    exit(0);

}