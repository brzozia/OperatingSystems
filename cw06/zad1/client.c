#include "common.h"
#define MSG_SIZE_MSG sizeof(struct chat_msg)-8
#define INPUT_SIZE 128

int my_id=-1;
int server_queue;
key_t my_key;
int friend_queue; 

struct chat_msg{
    long mtype;
    char msg[INPUT_SIZE];
};


int get_chat(int queue, struct chat_msg *Caa, int size, int type, int flag){
    return msgrcv(queue, Caa, size, type, flag);
}

void send_msg(int type, int connect_id){
    struct msgbufget msg;
    msg.mtype = type;
    msg.mkey=my_key;
    msg.msender_id=my_id;
    msg.mconnect_id=connect_id;

    if( msgsnd(server_queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){
        perror("error");
        exit(0) ;
    }
}


void send_chat(char *input, int queue){
    struct chat_msg sended;
    sended.mtype=MSG;
    strcpy(sended.msg,input);
    if( msgsnd(queue, &sended, MSG_SIZE_MSG, IPC_NOWAIT)==-1){
        perror("error - send chat");
        exit(0) ;
    }

}

int parse_input(char *input){
    if(strcmp(input, "LIST")==0)
        return LIST;
    else if(strcmp(input, "CONNECT")==0)
        return CONNECT;
    else if(strcmp(input, "DISCONNECT\n")==0)
        return DISCONNECT;
    else if(strcmp(input, "STOP")==0 || strcmp(input, "STOP\n")==0 )
        return STOP;
    else
        return MSG;
}


int chat_with_friend(int my_queue, int friend_queue, int who){
    printf("now you can chat with other client\n");

    char input[INPUT_SIZE];
    input[0]='\0';
    int type;
    struct chat_msg received;

    if(who==0){
        printf("enter message to client or DISCONNECT\n");
        while(strcmp(fgets(input, sizeof(input), stdin),"\n")==0);


        if((type=parse_input(input))!=MSG){
            return type;
        }
        send_chat(input,friend_queue);
        
    }
    
    while(1){
        if(get_chat(my_queue, &received, MSG_SIZE_MSG, MSG, IPC_NOWAIT)>0){
            printf("received: %s",received.msg);

            if(strcmp(received.msg,"DISCONNECT\n")==0)
                return DISCONNECT;
            if(strcmp(received.msg,"STOP\n")==0)
                return DISCONNECT;
            
            printf("enter message to client or DISCONNECT\n");
            while(strcmp(fgets(input, sizeof(input), stdin),"\n")==0);
           

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

    struct msqid_ds buff;
    rm_msg(my_id,IPC_RMID, &buff );
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
    server_queue = make_msg(server_key,S_IRWXU);

    if(queue==-1){
        perror("error during creating queue");
        return 1;
    }

    send_msg(INIT, -1);

    struct msgbufget msgg;

    while(1){

        if(get_msg(queue, &msgg, MSG_SIZE, INIT, 0)>0){
            my_id=msgg.msender_id;
            printf("my id %d\n", my_id);
            break;
        }
    }


    
    while(1){
        char input[15];
        scanf("%s", input);
        int type = parse_input(input);
        
        get_msg(queue, &msgg, MSG_SIZE, -10,  IPC_NOWAIT);
        
        //LIST
        if(type==LIST){

            send_msg(LIST, -1);
            
            char status[64];

            while(1){

                if(get_msg(queue, &msgg, MSG_SIZE, LIST, 0)>0){
                    if(msgg.msender_id==-1)
                        break;

                    if(msgg.mconnect_id==CONNECT)
                        strcpy(status, "not available for connection");
                    else
                        strcpy(status, "available for connection");

                    printf("client's id: %d, status: %s\n",msgg.msender_id, status);
                    
                }
            }
        }
        //CONNECT
        else if(type==CONNECT){
            int connect_id;
            scanf("%d", &connect_id);
            
            send_msg(CONNECT, connect_id);
            

            get_msg(queue, &msgg, MSG_SIZE, CONNECT, 0);                        // receives info from server
            
            friend_queue = make_msg(msgg.mkey,0);
            int next_type = chat_with_friend(queue, friend_queue,0);
            msgg.mtype=-1;
        
            if(next_type==DISCONNECT){                                              // disconnecting or stoping
                disconnect(my_id, server_queue);                
            }
            else if(next_type==STOP){
                exit(0);
            }
        }
        //STOP
        else if(msgg.mtype==STOP || type==STOP){
            
            exit(0);
        }
        //RECEIVE MSG
        else if(msgg.mtype==CONNECT){

            int desc= make_msg(msgg.mkey,0);
            int next_type = chat_with_friend(queue, desc, 1);
            msgg.mtype=-1;
           
            if(next_type==DISCONNECT){                                              // disconnecting or stoping
                disconnect(my_id, server_queue);
            }
            else if(next_type==STOP){
                exit(0);
            }
        }

    }
}