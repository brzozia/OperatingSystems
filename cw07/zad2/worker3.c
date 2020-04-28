#include "common.h"
sem_t *semaf[SEM_NO];
struct sh_struct *shared_struct;

char name[24];

void exitwork(){
    printf("end program %d\n", (int)getpid());

    for(int i=0;i<SEM_NO;i++){
        if(sem_close (semaf[i]) == -1 ){
        perror("sem close error\n");
        }
    }
    disconnect_memory(shared_struct);

}
void sig_handler(int hand){
    exitwork();
}




int main(int argc, char **argv){
    
    atexit(exitwork);
    signal(SIGINT, sig_handler);

    if(argc<1){
        printf("worker3 - Wrong numer of arguments\n");
    }

    int x=0,p=0,z;
    sem_get(O_RDWR);

    key_t memory_key = ftok(getenv("HOME"), 'l');
    sprintf(name, "/%d", (int)memory_key);
    int memory_id = shm_open( name, O_RDWR ,0666);
    if(memory_id==-1){
        perror("3 open memory error");
    }
    shared_struct=mmap( NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, memory_id,0);

    struct tm * timeinfo;
    struct timeval tim;


    while(1){
        
        sem_op(0,-1);

        if(shared_struct->sended3>=MAX_PRODUCTS){
            sem_op(0,1);
            disconnect_memory(shared_struct);
            exit(0);
        }


        int loop=0;
        while(shared_struct->loop3<ARRAY_SIZE){
            int j=shared_struct->loop3;
            if(shared_struct->array12[j].worker==2){
                p=shared_struct->array12[j].no*3;
                shared_struct->array12[j].no=-1;
                shared_struct->array12[j].worker=3;
                sem_op(2,-1);
                
                
                x=sem_ctl(2);
                z=sem_ctl(1);
                
                timeinfo=get_time();
                gettimeofday(&tim, NULL);
                printf("(%d, %d:%d:%d:%ld ) Wyslalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
                (int)getpid(),timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tim.tv_sec*1000 + tim.tv_usec/1000,p,z,x);
                shared_struct->sended3+=1;

                break;
            }
            shared_struct->loop3++;loop++;
            if(shared_struct->loop3==ARRAY_SIZE)
                shared_struct->loop3=0;
            if(loop==ARRAY_SIZE)
                break;
        }
        sem_op(0,1);
        sleep(1);

    }
    exit(0);

}