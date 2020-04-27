#include "common.h"

int main(int argc, char **argv){
    if(argc<2){
        printf("worker2 - Wrong numer of arguments\n");
    }

    int p=0,x=0,j=0,z;
    // struct semafores semafs;
    // for(int i=0;i<SEM_NO;i++){
    //     semafs.semaf[0]=
    // }
    // sscanf(argv[2], "%d", &sem_id);
    struct sh_struct *shared_struct=sh_memory(NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, argv[1],0);
    struct tm * timeinfo;
    struct timeval tim;

    while(1){

        sem_op(0,0,-1);
        if(shared_struct->prepared2>=MAX_PRODUCTS){
            // raise(SIGUSR1);
            sem_op(0,0,1);
            disconnect_memory(shared_struct);
            exit(0);
        }

        for(j=0;j<ARRAY_SIZE;j++){
            if(shared_struct->array12[j].worker==1){
                shared_struct->array12[j].no*=2;
                p=shared_struct->array12[j].no;
                shared_struct->array12[j].worker=2;
                sem_op(0,2,1);
                sem_op(0,1,-1);
                
                timeinfo=get_time();
                gettimeofday(&tim, NULL);
                x=sem_ctl(0,2);
                z=sem_ctl(0,1);
                printf("(%d, %d:%d:%d:%ld ) Przygotowalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
                (int)getpid(),timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tim.tv_usec/1000,p,z,x);
                shared_struct->prepared2++;

                break;
            }
           
        }
        sem_op(0,0,1);

        
    }

    disconnect_memory(shared_struct);
    exit(0);

}