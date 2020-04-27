#include "common.h"

int memory_id;
int sem_id;

void exit_func(){
    printf("\33[31m aaa zabili mnie %d \n \33[37m", (int)getpid());

    if(semctl (sem_id,IPC_RMID,0) == -1 ){
        perror("\033[31m blad zamykania semaforow\n");
    }
    if(shmctl(memory_id,IPC_RMID ,0) == -1){
        perror("\033[31m blad zamykania pamieci wspoldzielonej\n");
    }
   
}
void sig_handler(int hand){
    exit_func();
}

int main(){
    atexit(exit_func);
    signal(SIGINT, sig_handler);
    int sem_no=3; //one semafor to operate on arrays, one to count items to prepare, one to count items to send, one to count all sended items, ine to count all prepared items

    key_t memory_key = ftok(getenv("HOME"), 'm');
     memory_id = sh_get( memory_key,  sizeof(struct sh_struct),  IPC_CREAT | S_IRWXU);

    struct sh_struct *shared_struct=shmat( memory_id, NULL, S_IRWXU);
    shared_struct->made1=0;
    shared_struct->prepared2=0;
    shared_struct->sended3=0;
    for(int i=0;i<ARRAY_SIZE;i++){
        shared_struct->array12[i].no=-1;
    }
    disconnect_memory(shared_struct);

    char memory_str[11], sem_str[24];
    sprintf(memory_str, "%d",memory_id);
    int child_pid;

    key_t semafor_key = ftok(getenv("HOME"), 's');
     sem_id=sem_get(semafor_key, sem_no, IPC_CREAT | S_IRWXU); 
    sprintf(sem_str, "%d",sem_id);

    for(int i=0;i<sem_no;i++){
        sem_ctl(sem_id, i,SETVAL,0);
    }
    

    for(int i=0;i<NO_OF__ONE_TYPE_WORKERS;i++){
        child_pid=(int)fork();
        
        if(child_pid==0 ){
            execl("./worker1", "./worker1",memory_str, sem_str,NULL);
        }
            
        child_pid=(int)fork();
        if(child_pid==0 ){
            execl("./worker2", "./worker2",memory_str,sem_str,NULL);
        }
            
        child_pid=(int)fork();
        if(child_pid==0 )
            execl("./worker3", "./worker3",memory_str,sem_str,NULL);
    }
    while((child_pid=wait(NULL))!=-1);

}