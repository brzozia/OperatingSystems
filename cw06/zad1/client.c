#include "common.h"
#define MSG_SIZE_MSG sizeof(struct chat_msg)-8


int my_id;
int server_queue;
key_t my_key 

struct chat_msg{
    long mtype;
    char msg[256];
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
        return MSG;
}


int chat_with_friend(int my_queue, int friend_queue, int who){
    printf("now you can chat with other client\n");

    char input[256];
    int type;
    struct chat_msg sended, received;
    
    
    if(who==0){
        printf("enter message to client or DISCONNECT\n");
        scanf("%s", input);

        if((type=parse_input(input))!=MSG){
            return type;
        }
        
        sended.mtype=MSG;
        strcpy(sended.msg,input);
        if( msgsnd(friend_queue, &sended, MSG_SIZE_MSG, IPC_NOWAIT)==-1){
            perror("error");
            return 1;
        }

    }
    
    while(1){
        printf("czekam na wiadomość\n");
        if(msgrcv(my_queue, &received, MSG_SIZE_MSG, MSG, IPC_NOWAIT)!=0){
            printf("%s\n",received.msg);
            printf("enter message to client or DISCONNECT\n");
            scanf("%s", input);

            sended.mtype=type;
            strcpy(sended.msg,input);
            if( msgsnd(friend_queue, &sended, MSG_SIZE_MSG, IPC_NOWAIT)==-1){
                perror("error");
                return 1;
            }

            if((type=parse_input(input))!=MSG){
                return type;
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

void exit_function(){
    printf("zamykam sie\n");
    struct msgbuf msg;
    msg.mtype = STOP;
    msg.msender_id = my_id;

    if( msgsnd(server_queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){
        perror("error");
        return;
    }
    struct msqid_ds buff;
    msgctl(my_id,IPC_RMID, &buff );
}

void Chandler(int hand){
    exit(0);
}


int send_msg(int type, )){
    struct msgbuf first_msg;
    first_msg.mtype = INIT;
    first_msg.mkey=my_key;

    if( msgsnd(server_queue, &first_msg, MSG_SIZE, IPC_NOWAIT)==-1){
        perror("error");
        return 1;
    }
}

int get_msg(){

}


int main(){

    atexit(exit_function);
    signal(SIGINT, Chandler);
    my_key = ftok(getenv("HOME"),(int)getpid());
    key_t server_key = ftok(getenv("HOME"), 1);
    int queue = msgget(my_key, IPC_CREAT | S_IRWXU);
    server_queue = msgget(server_key,S_IRWXU);

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


    while(1){
        if(msgrcv(queue, &msgbufget, MSG_SIZE, INIT, 0)>0){
            my_id=msgbufget.msender_id;
            printf("my id %d\n", my_id);
            break;
        }
    }


    




    while(1){
        char input[15];

        msgrcv(queue, &msgbufget, MSG_SIZE, 0,  IPC_NOWAIT);
            
        scanf("%s", input);
        int type = parse_input(input);
        printf("type: %d\n",type);
        

        

        //LIST
        if(type==LIST){
            struct msgbuf msg;
            msg.mtype = LIST;
            msg.mkey=my_key;

            if( msgsnd(server_queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){
                perror("error send - LIST");
                return 1;
            }

            char status[64];

            while(1){

                if(msgrcv(queue, &msgbufget, MSG_SIZE, LIST, 0)>0){
                    if(msgbufget.msender_id==-1)
                        break;

                    if(msgbufget.mconnect_id==CONNECT)
                        strcpy(status, "not available for connection");
                    else
                        strcpy(status, "available for connection");

                    printf("client's id: %d, status: %s\n",msgbufget.msender_id, status);
                    
                }
            }

        }
        //CONNECT
        else if(type==CONNECT){
            int connect_id;
            scanf("%d", &connect_id);
            
            struct msgbuf msg;
            msg.mtype = CONNECT;
            msg.msender_id=my_id;
            msg.mconnect_id = connect_id;
            msg.mkey=my_key;

            if( msgsnd(server_queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){              // sends info to server
                perror("send error - CONNECT");
                return 1;
            }

            msgrcv(queue, &msgbufget, MSG_SIZE, CONNECT, 0);                        // receives info from server
            int friend_queue = msgget(msgbufget.mkey,0);
            int next_type = chat_with_friend(queue, friend_queue,0);


            if(next_type==DISCONNECT){                                              // disconnecting or stoping
                disconnect(my_id, connect_id, server_queue);
            }
            else if(next_type==STOP){

            }
        }
        else if(type==STOP){
            exit(0);

        }
        else if(msgbufget.mtype==CONNECT){
            chat_with_friend(queue, msgget(msgbufget.mkey,0), 1);
        }

    }
}