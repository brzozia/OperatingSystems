#include "common.h"


struct client_info{
    key_t key;
    int status;
};



int main(){

    struct client_info clients[CLIENTS_NO];
    int full=0;
    int real_clients_no=0;
    key_t server_key = ftok(getenv("HOME"), 1);
    int server_queue = msgget(server_key, IPC_CREAT | S_IRWXU );
    


    if(server_queue==-1){
        perror("error during during creating queue");
        return 1;
    }

    struct msgbufget msgbufget;
    struct msgbuf msg; 
    msgbufget.mtype=-1;



    while(1){
        while(msgrcv(server_queue, &msgbufget, MSG_SIZE, 0, 0)<=0);

        //printf("server lol %ld",msgbufget.mtype);

        //INIT
        if(msgbufget.mtype==INIT){
            
            if(real_clients_no==CLIENTS_NO || full==1){                 //when list of clients is full checks stopped clients
                full=1;
                for(int i=0;i<real_clients_no;i++){
                    if(clients[i].status==STOP){
                        real_clients_no=i;
                        break;
                    }
                }
                if(real_clients_no==CLIENTS_NO){
                    printf("Server: List of clients if full");
                    break;
                }
            }

            struct client_info client_info;                             // adds client's data to table
            client_info.key=msgbufget.mkey;
            client_info.status=INIT;
            clients[real_clients_no]=client_info;

            int queue = msgget(client_info.key,S_IRWXU);                      // opens client's queue
            
            struct msgbuf msg;                                         // prepares return message
            msg.mtype = INIT;
            msg.msender_id=real_clients_no;
            real_clients_no++;

            if( msgsnd(queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){         // sends to client it's id
                perror("error");
                return 1;
            }
        }
        //LIST
        else if(msgbufget.mtype==LIST){
            
            if(full==1)
                real_clients_no=CLIENTS_NO;

            int queue = msgget(msgbufget.mkey,S_IRWXU);

            for(int i=0;i<real_clients_no;i++){                         // prints informations about not-stopped clients

                if(clients[i].status!=STOP){
                    msg.mtype = LIST;
                    msg.msender_id=i;
                    msg.mconnect_id = clients[i].status;

                    if( msgsnd(queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){         // sends to client it's id
                        perror("error");
                        return 1;
                    }
                }   
            }
            msg.mtype = LIST;
            msg.msender_id=-1;
            if( msgsnd(queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){         // sends to client it's id
                perror("error");
                return 1;
            }
        }
        //CONNECT
        else if(msgbufget.mtype==CONNECT){
            if(clients[msgbufget.mconnect_id].status==CONNECT || clients[msgbufget.mconnect_id].status==STOP){
                printf("Server: Client which who you want to connect is not available\n");
                break;
            }

            int queue = msgget(msgbufget.mkey, S_IRWXU);                      // opens client's queue
            
            struct msgbuf msg;                                          // prepares return message
            msg.mtype = CONNECT;
            msg.mkey = clients[msgbufget.mconnect_id].key;

            if( msgsnd(queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){         // sends to client key to connect
                perror("send error - queue");
                return 1;
            }

            int queue2 = msgget(msg.mkey, S_IRWXU);
            msg.mkey = clients[msgbufget.msender_id].key;
            
            if( msgsnd(queue2, &msg, MSG_SIZE, IPC_NOWAIT)==-1){         // sends to client key to connect
                perror("send error - queue2");
                return 1;
            }

            clients[msgbufget.msender_id].status=CONNECT;
            clients[msgbufget.mconnect_id].status=CONNECT;

        }
        //DISCONNECT
        else if(msgbufget.mtype==DISCONNECT){
            clients[msgbufget.msender_id].status=DISCONNECT;
            clients[msgbufget.mconnect_id].status=DISCONNECT;
        }
        //STOP
        else if(msgbufget.mtype==STOP){
            clients[msgbufget.msender_id].status=STOP;



        }
    }



}