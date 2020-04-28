#include "common.h"

char name[24];

void exit_func(void){
    printf("end program %d\n", (int)getpid());
    for(int i=0;i<SEM_NO;i++){
        if(sem_close (semaf[i]) == -1 ){
        perror(" sem close error\n");
        }
    sem_unlink(names[i]);

    }
    
    if(shm_unlink(name) == -1){
        perror("shm unlink error\n");
    }
   
}
void sig_handler(int hand){
    exit_func();
}




int main(){
    atexit(exit_func);
    signal(SIGINT, sig_handler);
    key_t memory_key = ftok(getenv("HOME"), 'l');
    
    sprintf(name, "/shmm%d", (int)memory_key);
    int memory_id = shm_open( name,  O_CREAT | O_RDWR | O_TRUNC,0666);

    if(memory_id==-1){
        perror("shm_opern error");
    }
    int size=sizeof(struct sh_struct);
    if(ftruncate( memory_id, size)==-1){
        perror("fruncate erroe");
    }

    struct sh_struct *shared_struct=mmap( NULL, sizeof(struct sh_struct), PROT_READ |PROT_EXEC |PROT_WRITE ,MAP_SHARED, memory_id,0);
    if(shared_struct==(void*)-1){
        perror("mmap program");
    }
    shared_struct->made1=0;
    shared_struct->prepared2=0;
    shared_struct->sended3=0;
    for(int i=0;i<ARRAY_SIZE;i++){
        shared_struct->array12[i].no=-1;
    }
    disconnect_memory(shared_struct);

    char memory_str[36];
    sprintf(memory_str, "%d",memory_id);
    int child_pid;

    

    sem_get( SEM_NO, O_CREAT ); 
    

    // char array[SEM_NO+3][64], sem_str[24];
    // strcpy(array[1],memory_str);
    // int i;
    // for(i=0;i<SEM_NO;i++){
    //     strcpy(sem_str,"");
    //     sprintf(sem_str, "%d",sem_id.semaf[i]);
    //     strcpy(array[2+i],sem_str);
    // }
    // strcpy(array[i],NULL);

    // for(int i=0;i<SEM_NO;i++){
    //     sem_ctl(sem_id, i,);
    // }
    

    for(int i=0;i<NO_OF__ONE_TYPE_WORKERS;i++){
        // child_pid=(int)fork();
        
        // if(child_pid==0 ){
            execl("./worker1", "./worker1",memory_str,"",NULL);
        // }

        child_pid=(int)fork();
        if(child_pid==0 ){printf("klocki");
            execl("./worker2","./worker2",memory_str, NULL);
       }
            
        child_pid=(int)fork();
        if(child_pid==0 ){printf("klocki");
            execl("./worker3","./worker3" ,memory_str,NULL);
       }
    }
    while((child_pid=wait(NULL))!=-1);

}