#include "common.h"

char sname[24];
struct sh_struct *shared_struct;

void exit_func(void){
    printf("end program %d\n", (int)getpid());
    sem_get(0,0);

    for(int i=0;i<SEM_NO;i++){
        if(sem_close (semaf[i]) == -1 ){
        perror(" sem close error\n");
        }
    sem_unlink(names[i]);

    }

    disconnect_memory(shared_struct);
    key_t memory_key = ftok(getenv("HOME"), 'l');
    sprintf(sname, "/%d", (int)memory_key);

    if(shm_unlink(sname) == -1){
        perror("shm unlink error\n");
    }
}

void sig_handler(int hand){
    exit_func();
}




int main(){
    // atexit(exit_func);
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

    

    sem_get( SEM_NO, O_RDWR|O_CREAT ); 
    

    for(int i=0;i<1;i++){
        child_pid=(int)fork();
        
        if(child_pid==0){
            printf("halo to ja1");
            execl("./worker1", "./worker1",memory_str,"",NULL);
        }

        child_pid=(int)fork();
        if(child_pid==0 ){
            printf("halo to ja2");
            execl("./worker2","./worker2",memory_str, "",NULL);
       }
            
        child_pid=(int)fork();
        if(child_pid==0 ){
            printf("halo to ja");
            execl("./worker3","./worker3",memory_str, "",NULL);
       }
    }
    while((child_pid=wait(NULL))!=-1);

}