#include "common.h"

char name[24];


void exit_func(void){
    printf("zabili mnie %d\n", (int)getpid());
    for(int i=0;i<SEM_NO;i++){
        if(sem_close (semaf[i]) == -1 ){
        perror(" blad zamykania semaforow\n");
    }

    }
    
    if(shm_unlink(name) == -1){
        perror("blad zamykania pamieci wspoldzielonej\n");
    }
   
}
void sig_handler(int hand){
    exit_func();
}




int main(){
    atexit(exit_func);
    signal(SIGINT, sig_handler);
    key_t memory_key = ftok(getenv("HOME"), 'l');
    sprintf(name, "/%d", (int)memory_key);
     memory_id = shm_open( name,  O_CREAT | O_RDWR | O_EXCL,666);

    if(memory_id==-1){
        perror("shm_opern error");
    }
    int size=sizeof(struct sh_struct);
    printf("%d",size);
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

    char memory_str[11];
    sprintf(memory_str, "%d",memory_id);
    int child_pid;

    

    sem_get( SEM_NO, O_CREAT | S_IRWXU); 
    

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
        child_pid=(int)fork();
        
        if(child_pid==0 ){
            // strcpy(array[0],"./worker1");
            execl("./worker1", "./worker1",memory_str,NULL);
        }
            
        child_pid=(int)fork();
        if(child_pid==0 ){
            // strcpy(array[0],"./worker2");
            execl("./worker2","./worker1",memory_str, NULL);
        }
            
        child_pid=(int)fork();
        if(child_pid==0 ){
            // strcpy(array[0],"./worker3");
            execl("./worker3","./worker1" ,memory_str,NULL);
        }
    }
    while((child_pid=wait(NULL))!=-1);

}