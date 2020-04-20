#include "common.h"
#define INPUT_SIZE 256

int my_id=-1;
int server_queue;
key_t my_key;
int friend_queue; 



void send_msg(int type, int connect_id){
    struct msgbufget msg;
    msg.mtype = type;
    msg.mkey=my_key;
    msg.msender_id=my_id;
    msg.mconnect_id=connect_id;

    if( mq_send(server_queue,(char *)  &msg, sizeof(msg), 10-type)==-1){
        perror("error");
        exit(0) ;
    }
}



void send_chat(char *input, int queue){
    struct msgbufget sended;
    sended.mtype=MSG;
    strcpy(sended.msg,input);
    if( mq_send(queue,(char *)  &sended,sizeof(sended), 10-MSG)==-1){
        perror("error - send chat");
        exit(0) ;
    }

}



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

void ChatChandler(int hand){
    send_chat("DISCONNECT",friend_queue);
    mq_close(friend_queue);

    exit(0);
}

int chat_with_friend(int my_queue, int friend_queue, int who){
    signal(SIGINT, ChatChandler);
    printf("now you can chat with other client\n");

    char input[INPUT_SIZE];
    int type;
    struct msgbufget received;
    
    if(who==0){
        printf("enter message to client or DISCONNECT\n");
        //  getline(&input, )
        scanf("%s",input);

        if((type=parse_input(input))!=MSG){
            return type;
        }
        send_chat(input,friend_queue);
    }
    
    while(1){
        if(get_msg(my_queue, &received, sizeof(received), MSG, 0)>0){
            printf("received: %s\n",received.msg);

            if(strcmp(received.msg,"DISCONNECT")==0)
                return DISCONNECT;
            if(strcmp(received.msg,"STOP")==0)
                return DISCONNECT;
            
            printf("enter message to client or DISCONNECT\n");
            //  fgets(input, INPUT_SIZE, stdin);
             scanf("%s",input);

            send_chat(input,friend_queue);

            if((type=parse_input(input))!=MSG){
                return type;
            }
                       
        }
    }
}




void disconnect(int my_id, int server_queue){
    send_msg(DISCONNECT, -1);
}



void exit_function(){
    printf("zamykam sie\n");

    send_msg(STOP, -1);

    mq_close(server_queue);
    struct msqid_ds buff;
    rm_msg(my_id,IPC_RMID, &buff );

    char name[24];
    sprintf(name, "/%d", (int)my_key);

    mq_unlink(name);
}

void Chandler(int hand){
    exit(0);
}







int main(){

    atexit(exit_function);
    signal(SIGINT, Chandler);
    my_key = ftok(getenv("HOME"),(int)getpid());
    key_t server_key = ftok(getenv("HOME"), 1);
    int queue = make_msg(my_key, IPC_CREAT | S_IRWXU);
    server_queue = make_msg(server_key,0);

    if(queue==-1){
        perror("error during creating queue");
        return 1;
    }

    send_msg(INIT, -1);

    struct msgbufget msg;

    while(1){
        if(get_msg(queue, &msg, sizeof(msg), INIT, 0)>0){
            my_id=msg.msender_id;
            printf("my id %d\n", my_id);
            break;
        }
    }



    
    while(1){
        char input[15];
        scanf("%s", input);
        int type = parse_input(input);
        
        get_msg(queue, &msg, sizeof(msg), -10,  IPC_NOWAIT);
        
        //LIST
        if(type==LIST){

            send_msg(LIST, -1);
            
            char status[64];

            while(1){

                if(get_msg(queue, &msg, sizeof(msg), LIST, 0)>0){
                    if(msg.msender_id==-1)
                        break;

                    if(msg.mconnect_id==CONNECT)
                        strcpy(status, "not available for connection");
                    else
                        strcpy(status, "available for connection");

                    printf("client's id: %d, status: %s\n",msg.msender_id, status);
                    
                }
            }

        }
        //CONNECT
        else if(type==CONNECT){
            int connect_id;
            scanf("%d", &connect_id);
            
            send_msg(CONNECT, connect_id);
            

            get_msg(queue, &msg, sizeof(msg), CONNECT, 0);                        // receives info from server
            friend_queue = make_msg(msg.mkey,0);
            int next_type = chat_with_friend(queue, friend_queue,0);
            mq_close(friend_queue);

        
            if(next_type==DISCONNECT){                                              // disconnecting or stoping
                disconnect(my_id, server_queue);
            }
            else if(next_type==STOP){
                exit(0);
            }
        }
        //STOP
        else if(msg.mtype==STOP || type==STOP){
            exit(0);
        }
        else if(msg.mtype==CONNECT){
            int fd = make_msg(msg.mkey,0);
            int next_type = chat_with_friend(queue, fd, 1);

            if(next_type==DISCONNECT){                                              // disconnecting or stoping
                disconnect(my_id, server_queue);
                mq_close(fd);
            }
            else if(next_type==STOP){
                exit(0);
            }
        }

    }
}