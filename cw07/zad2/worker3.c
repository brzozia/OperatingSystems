#include "common.h"

int main(int argc, char **argv){
    if(argc<2){
        printf("worker3 - Wrong numer of arguments\n");
    }

    int sem_id,x=0,p=0,z,j=0;
    sscanf(argv[2], "%d", &sem_id);
    struct sh_struct *shared_struct=sh_memory(NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, argv[1],0);
    struct tm * timeinfo;
    struct timeval tim;

    while(1){
        

        sem_op(0,0,-1);

        if(shared_struct->sended3>=MAX_PRODUCTS){
            sem_op(0,0,1);
            disconnect_memory(shared_struct);
            exit(0);
        }

        // sem_op(sem_id,0,-1);

        for(j=0;j<ARRAY_SIZE;j++){
            if(shared_struct->array12[j].worker==2){

                p=shared_struct->array12[j].no*3;
                shared_struct->array12[j].no=-1;
                shared_struct->array12[j].worker=3;
                sem_op(0,2,-1);
                
                timeinfo=get_time();
                gettimeofday(&tim, NULL);
                x=sem_ctl(0,2);
                z=sem_ctl(0,1);
                

                printf("(%d, %d:%d:%d:%ld ) Wyslalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
                (int)getpid(),timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tim.tv_usec/1000,p,z,x);
                shared_struct->sended3+=1;

                break;
            }
        }
        sem_op(0,0,1);

    }
    disconnect_memory(shared_struct);
    exit(0);

}