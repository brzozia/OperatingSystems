#include "common.h"
sem_t *semaf[SEM_NO];
struct sh_struct *shared_struct;

void exitwork(){
    disconnect_memory(shared_struct);
}

int main(int argc, char **argv){
    printf("!2!");
    atexit(exitwork);
    if(argc<2){
        printf("worker2 - Wrong numer of arguments\n");
    }

    int p=0,x=0,z;
    char name[24];
    // struct sh_struct *shared_struct=sh_memory(NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, argv[1],0);

sem_get(0, O_RDWR | O_EXCL);
// printf("%s,%s,%s",names[0],names[1],names[2]);

     key_t memory_key = ftok(getenv("HOME"), 'l');
    sprintf(name, "/%d", (int)memory_key);
    int memory_id = shm_open( name,  O_RDWR | O_EXCL ,0666);
    if(memory_id==-1){
        perror("2 open memory error");
    }
    shared_struct=mmap( NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, memory_id,0);
   
    
    struct tm * timeinfo;
    struct timeval tim;

    while(1){

        sem_op(0,0,-1);
        // printf("w2 %d", sem_ctl(0,0));
        if(shared_struct->prepared2>=MAX_PRODUCTS){
            // raise(SIGUSR1);
            sem_op(0,0,1);
            disconnect_memory(shared_struct);
            exit(0);
        }
        int loop=0;
        while(shared_struct->loop2<ARRAY_SIZE){
            int j=shared_struct->loop2;
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
            shared_struct->loop2++;loop++;
            if(shared_struct->loop2==ARRAY_SIZE)
                shared_struct->loop2=0;
            if(loop==ARRAY_SIZE)
                break;
           
        }
        sem_op(0,0,1);
        
    }

    // disconnect_memory(shared_struct);
    exit(0);

}