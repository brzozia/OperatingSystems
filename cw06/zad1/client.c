#include "common.h"


int main(){

    int my_id;
    key_t my_key = ftok(getenv("HOME"),(int)getpid());
    key_t server_key = ftok(getenv("HOME"), 's');
    int queue = msgget(my_key, IPC_CREAT | S_IRWXU);
    int server_queue = msgget(server_key,0);

    if(queue==-1){
        perror("error during creating queue");
        return 1;
    }

    struct msgbuf first_msg;
    first_msg.mtype = INIT;
    first_msg.mkey=my_key;

    if( msgsnd(server_queue, &first_msg, MSG_SIZE, IPC_NOWAIT)==-1){
        perror("error");
        return 1;
    }

    struct msgbufget msgbufget;
    msgrcv(queue, &msgbufget, MSG_SIZE, INIT, 0);
    my_id=msgbufget.msender_id;
}