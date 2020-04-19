#include "common.h"


struct client_info{
    key_t key;
    int status;
};

int main(){

    struct client_info clients[CLIENTS_NO];
    int full=0;
    int real_clients_no=0;
    key_t server_key = ftok(getenv("HOME"), 's');
    int server_queue = msgget(server_key, IPC_CREAT | S_IRWXU );

    if(server_queue==-1){
        perror("error during during creating queue");
        return 1;
    }


    while(1){
        struct msgbufget msgbufget;
        msgrcv(server_queue, &msgbufget, MSG_SIZE, INIT, 0);

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

            int queue = msgget(client_info.key,0);                      // opens client's queue
            
            struct msgbuf msg;;                                         // prepares return message
            msg.mtype = INIT;
            msg.msender_id=real_clients_no;

            if( msgsnd(queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){         // sends to client it's id
                perror("error");
                return 1;
            }
        }
        //LIST
        else if(msgbufget.mtype==LIST){
            char status[30];

            if(full=1)
                real_clients_no==CLIENTS_NO;


            for(int i=0;i<real_clients_no;i++){                         // prints informations about not-stopped clients
                
                if(clients[i].status==CONNECT)
                    strcpy(status, "not available for connection");
                else
                    strcpy(status, "available for connection");

                if(clients[i].status!=STOP) 
                printf("client's id: %d, status: %s\n",i, status);
            }
        }
        //CONNECT
        else if(msgbufget.mtype==CONNECT){
            int queue = msgget(msgbufget.mkey, 0);                      // opens client's queue
            
            struct msgbuf msg;                                         // prepares return message
            msg.mtype = CONNECT;
            msg.msender_id=real_clients_no;
        }
        //DISCONNECT
        else if(msgbufget.mtype==DISCONNECT){

        }
        //STOP
        else if(msgbufget.mtype==DISCONNECT){

        }
    }
}