#include "common.h"

struct sh_struct *shared_struct;
// int sem_id;

// void exit1(void){
//     sem_op(sem_id,0,1);
//     disconnect_memory(shared_struct);
//     exit(0);
   
// }
// void hand1(int hand){
//     exit1();
// }

int main(int argc, char **argv){
    // signal(SIGUSR1, hand1);
    if(argc<2){
        printf("worker1 - Wrong numer of arguments\n");
    }
printf("hello");
    int x=0,i, z,j;
    struct tm * timeinfo;
    struct timeval tim;
    // sscanf(argv[2], "%d", &sem_id);
    shared_struct=sh_memory(NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, argv[1],0);

    
    while(1){
        printf("hello");
        if(shared_struct->made1!=0)
            sem_op(0,0,-1);

        if(shared_struct->made1>=MAX_PRODUCTS){
            // raise(SIGUSR1);
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