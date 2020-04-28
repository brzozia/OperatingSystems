#include "common.h"
//program makes small "jumps" during writing output on console - that is because every process=worker has to sleep(1) after adding package


char sname[24];
struct sh_struct *shared_struct;


void exit_func(void){
    printf("prog end program \n");

    for(int i=0;i<SEM_NO;i++){
        if(sem_close (semaf[i]) == -1 ){
        perror("prog sem close error\n");
        }
        if(sem_unlink(names[i])==-1){
            perror("prog sem unlink error\n");
        }

    }

    disconnect_memory(shared_struct);
    key_t memory_key = ftok(getenv("HOME"), 'l');
    sprintf(sname, "/%d", (int)memory_key);

    if(shm_unlink(sname) == -1){
        perror("prog shm unlink error\n");
    }
}

void sig_handler(int hand){
    sleep(1);
    exit(0);
}




int main(){
    atexit(exit_func);
    signal(SIGINT, sig_handler);
    key_t memory_key = ftok(getenv("HOME"), 'l');
    
    sprintf(sname, "/%d", (int)memory_key);
    int memory_id = shm_open( sname,  O_CREAT | O_RDWR | O_TRUNC,0666);

    if(memory_id==-1){
        perror("shm_opern error");
    }
    int size=sizeof(struct sh_struct);
    if(ftruncate( memory_id, size)==-1){
        perror("fruncate erroe");
    }

    shared_struct=mmap( NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, memory_id,0);
    if(shared_struct==(void*)-1){
        perror("mmap program");
    }
    shared_struct->made1=0;
    shared_struct->prepared2=0;
    shared_struct->sended3=0;
    shared_struct->loop1=0;
    shared_struct->loop2=0;
    shared_struct->loop3=0;
    for(int i=0;i<ARRAY_SIZE;i++){
        shared_struct->array12[i].no=-1;
    }
    
    char memory_str[36];
    sprintf(memory_str, "%d",memory_id);
    int child_pid;

    

    sem_get( O_RDWR|O_CREAT ); 
    

    for(int i=0;i<NO_OF__ONE_TYPE_WORKERS;i++){
        child_pid=(int)fork();
        
        if(child_pid==0){
            execl("./worker1", "./worker1",NULL);
        }

        child_pid=(int)fork();
        if(child_pid==0 ){
            execl("./worker2","./worker2",NULL);
       }
            
        child_pid=(int)fork();
        if(child_pid==0 ){
            execl("./worker3","./worker3",NULL);
       }
    }
    while((child_pid=wait(NULL))!=-1);

}