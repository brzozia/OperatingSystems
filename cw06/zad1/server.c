#include "common.h"


struct client_info{
    key_t key;
    int status;
    int decr;
};

key_t server_key;
int real_clients_no=0;
int full=0;
struct client_info clients[CLIENTS_NO];
int server_queue;



void send_msg(int type, int connect_id, int sender_id, key_t key, int queue){
    struct msgbufget msg;
    msg.mtype = type;
    msg.mkey=key;
    msg.msender_id=sender_id;
    msg.mconnect_id=connect_id;

    if( msgsnd(queue, &msg, MSG_SIZE, IPC_NOWAIT)==-1){
        perror("error");
        return;
    }
}

void exit_function(){
    printf("server goes home\n");
    if(full==1)
        real_clients_no=CLIENTS_NO;
    
    int received=0;
    struct msgbufget msgg;

    for(int i=0;i<real_clients_no;i++){                         // prints informations about not-stopped clients
        if(clients[i].status!=STOP){
            send_msg(STOP,-1, -1,-1,clients[i].decr);
            while(get_msg(server_queue, &msgg, MSG_SIZE, STOP, 0)<=0);
                
            if(msgg.mconnect_id==i)
                received++;
        }     
    }

    struct msqid_ds buff;
    if(rm_msg(server_queue,IPC_RMID, &buff )){
        perror("remove queue");
    }
    
}


void Chandler(int hand){
    exit(0);
}

int main(){

    atexit(exit_function);
    signal(SIGINT, Chandler);    
    real_clients_no=0;
    server_key = ftok(getenv("HOME"), 1);
    server_queue = make_msg(server_key, IPC_CREAT | S_IRWXU );

    if(server_queue==-1){
        perror("error during creating queue");
        return 1;
    }

    struct msgbufget msgbufget;
    msgbufget.mtype=-1;



    while(1){
        while(get_msg(server_queue, &msgbufget, MSG_SIZE, -10, 0)<=0);


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
            int queue = make_msg(client_info.key,S_IRWXU); 
             client_info.decr=queue;
            clients[real_clients_no]=client_info;

            
            send_msg(INIT, -1, real_clients_no, -1, queue);
            
            real_clients_no++;
        }
        //LIST
        else if(msgbufget.mtype==LIST){
            
            if(full==1)
                real_clients_no=CLIENTS_NO;

            int queue = clients[msgbufget.msender_id].decr;//make_msg(msgbufget.mkey,0);

            for(int i=0;i<real_clients_no;i++){                         // prints informations about not-stopped clients

                if(clients[i].status!=STOP){
                    send_msg(LIST,clients[i].status, i,-1,queue);
                }   
            }
            send_msg(LIST, -1,-1,-1,queue);
           
        }
        //CONNECT
        else if(msgbufget.mtype==CONNECT){
            if(clients[msgbufget.mconnect_id].status==CONNECT || clients[msgbufget.mconnect_id].status==STOP){
                printf("Server: Client which who you want to connect is not available\n");
                break;
            }

            int queue = make_msg(msgbufget.mkey, 0);  //clients[msgbufget.msender_id].decr;//                     // opens client's queue
            send_msg(CONNECT,-1,-1,clients[msgbufget.mconnect_id].key,queue);
            

            int queue2 = make_msg(clients[msgbufget.mconnect_id].key, 0);//clients[msgbufget.mconnect_id].decr;
            send_msg(CONNECT,-1,-1,clients[msgbufget.msender_id].key,queue2);
            

            clients[msgbufget.msender_id].status=CONNECT;
            clients[msgbufget.mconnect_id].status=CONNECT;

        }
        //DISCONNECT
        else if(msgbufget.mtype==DISCONNECT){
            clients[msgbufget.msender_id].status=DISCONNECT;
        }
        //STOP
        else if(msgbufget.mtype==STOP){
            clients[msgbufget.msender_id].status=STOP;
        }
    }



}