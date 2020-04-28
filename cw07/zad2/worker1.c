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
        printf("worker1 - Wrong numer of arguments\n");
    }

    int x=0,i, z,zarodek;
    time_t tt;
    zarodek = time(&tt);
    srand(zarodek); 
    struct tm * timeinfo;
    struct timeval tim;

    sem_get( O_RDWR );

    
    key_t memory_key = ftok(getenv("HOME"), 'l');
    sprintf(name, "/%d", (int)memory_key);
    int memory_id = shm_open( name,  O_RDWR  ,0666);
    if(memory_id==-1){
        perror("1 open memory error");
    }
    shared_struct=mmap( NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, memory_id,0);

    
    while(1){

        sem_op(0,-1);

        if(shared_struct->made1>=MAX_PRODUCTS){
            sem_op(0,1);
            disconnect_memory(shared_struct);
            exit(0);
        }

        i=rand()%100;
        
        int loop=0;
        while(loop<ARRAY_SIZE){

            int j=shared_struct->loop1;
            if(shared_struct->array12[j].no==-1){

                shared_struct->array12[j].no=i;
                shared_struct->array12[j].worker=1;   
                
                sem_op(1,1);             
                break;
            }

            shared_struct->loop1++;loop++;
            if(shared_struct->loop1==ARRAY_SIZE)
                shared_struct->loop1=0;
            
            
        }
        if(loop<ARRAY_SIZE){
            
            x=sem_ctl(2);
            z=sem_ctl(1);

            shared_struct->made1++;
            
            timeinfo=get_time();
            gettimeofday(&tim, NULL);
            printf("(%d, %d:%d:%d:%ld ) Dodalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
                    (int)getpid(),timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tim.tv_sec*1000 + tim.tv_usec/1000,i,z,x);
        
        }
        sem_op(0,1);
        sleep(1);
        
    }
    exit(0);
}