#include "common.h"

struct chat_msg{
    long mtype;
    char msg[MSG_SIZE];
};


int parse_input(char *input){
    if(strcmp(input, "LIST")==0)
        return LIST;
    else if(strcmp(input, "CONNECT")==0)
        return CONNECT;
    else if(strcmp(input, "DISCONNECT")==0)
        return DISCONNECT;
    else if(strcmp(input, "STOP")==0)
        return STOP;
    else
        return 0;
}


int chat_with_friend(int my_queue, int friend_queue){
    printf("now you can chat with other client\n");

    char input[256];
    int type;
    printf("enter message to client or DISCONNECT\n");
    scanf("%s", input);

    if((type=parse_input(input))!=MSG){
        return type;
    }


    struct chat_msg sended, received;
    sended.mtype=MSG;
    strcpy(sended.msg,input);
    if( msgsnd(friend_queue, &sended, MSG_SIZE, IPC_NOWAIT)==-1){
        perror("error");
        return 1;
    }

    while(1){
        if(msgrcv(my_queue, &received, MSG_SIZE, MSG, 0)!=0){

            printf("enter message to client or DISCONNECT\n");
            scanf("%s", input);
            if((type=parse_input(input))!=MSG){
                return type;
            }

            strcpy(sended.msg,input);
            if( msgsnd(friend_queue, &sended, MSG_SIZE, IPC_NOWAIT)==-1){
                perror("error");
                return 1;
            }
        }
    }
}


void disconnect(int my_id, int connect_id, int server_queue){
    struct msgbuf msg;
    msg.mtype = DISCONNECT;
    msg.msender_id = my_id;
    msg.mconnect_id = connect_id;

    if( msgsnd(server_queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){
        perror("error");
        return;
    }
}





int main(){

    int my_id;
    key_t my_key = ftok(getenv("HOME"),(int)getpid());
    key_t server_key = ftok(getenv("HOME"), 's');
    int queue = msgget(my_key, IPC_CREAT | S_IRWXU);
    int server_queue = msgget(server_key,S_IRWXU);

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
    while(msgrcv(queue, &msgbufget, MSG_SIZE, INIT, 0)==0);
    
    my_id=msgbufget.msender_id;



    struct msgbuf msg;


    while(1){
        char input[15];
        scanf("%s", input);
        int type = parse_input(input);

        //LIST
        if(type==LIST){
            msg.mtype = LIST;

            if( msgsnd(server_queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){
                perror("error");
                return 1;
            }

        }
        //CONNECT
        else if(type==CONNECT){
            int connect_id;
            scanf("%d", &connect_id);
            
            msg.mtype = CONNECT;
            msg.msender_id=my_id;
            msg.mconnect_id = connect_id;
            msg.mkey=my_key;

            if( msgsnd(server_queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){              // sends info to server
                perror("error");
                return 1;
            }

            msgrcv(queue, &msgbufget, MSG_SIZE, CONNECT, 0);                        // receives info from server
            int friend_queue = msgget(msgbufget.mkey,S_IRWXU);
            int next_type = chat_with_friend(queue, friend_queue);


            if(next_type==DISCONNECT){                                              // disconnecting or stoping
                disconnect(my_id, connect_id, server_queue);
            }
            else if(next_type==STOP){

            }
        }
        else if(type==STOP){

        }
        else if(type==MSG){
            chat_with_friend(queue, msgget(msgbufget.mkey,S_IRWXU));
        }

    }
}