#include "common.h"

sem_t semaf;

struct client_info{
    char name[32];
    struct sockaddr desc;
    int playing_with;
    int is_working;
    int ping;
    int socket_type;
};

struct client_info *clients;
int fill;
int undesc;
int netdesc;
int epoldesc;
char path[10];
pthread_t pthread_id;



int add_client(struct client_info client){
    if(fill<MAX_CLIENTS){
        clients[fill++]=client;
        return 0;
    }
   
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].is_working!=CONNECT){
            clients[i]=client;
            return 0;
        }
    }
    return ERROR;
}



int find_client_using_name(char *name){
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].is_working==CONNECT && strcmp(name, clients[i].name)==0)
            return i;
    }
    return ERROR;
}


int find_client_using_desc(struct sockaddr desc){
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].is_working==CONNECT && clients[i].desc.sa_data==desc.sa_data)
        return i;
    }
    return ERROR;
}

int find_free_client(char *name1){
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].is_working==CONNECT && clients[i].playing_with==-1 && clients[i].name!=name1)
        return i;
    }
    return ERROR;
}
void sig_handler(int num){
    exit(0);
}

void exit_handler(void){
    printf("closing server\n");
    struct message msg;

    pthread_cancel(pthread_id);

    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].is_working==CONNECT){
            
            msg.type=DISCONNECT;
            sendto(clients[i].socket_type, &msg, msg_size,0,&clients[i].desc, sizeof(clients[i].desc));
        }
    }

    sem_destroy(&semaf);
    close(netdesc);
    close(undesc);
    unlink(path);
    free(clients);
}



void connect_procedure(char *name, int client_id, int second_client){

    struct message msg;
    clients[second_client].playing_with=client_id;
    clients[client_id].playing_with=second_client;
    msg.other = rand() % 1;

    strcpy(msg.name,clients[second_client].name);
    msg.msg=O;
    msg.type=CONNECT;
    if(sendto(clients[client_id].socket_type,&msg, msg_size,0,&clients[client_id].desc, sizeof(clients[client_id].desc))==-1)
        perror("server: send info 1 error");
    
    strcpy(msg.name,clients[client_id].name);
    msg.msg=X;
    if(sendto(clients[second_client].socket_type,&msg, msg_size,0,&clients[second_client].desc, sizeof(clients[second_client].desc))==-1)
        perror("server: send info 2 error");
    

}


void unexpected_client_exit(int connected_client){
    
    struct message msg1;
    msg1.msg=WAITING_FOR_PLAYER;
    msg1.type=CONNECT;
    msg1.other=ERROR;
    if(sendto(clients[connected_client].socket_type, &msg1, msg_size,0,&clients[connected_client].desc, sizeof(clients[connected_client].desc))==-1)
        perror("server: unexpected send waiting for player error");
    

    int second_client = find_free_client(clients[connected_client].name);

    if(second_client != ERROR)
        connect_procedure(clients[connected_client].name, connected_client,second_client);

}

void disconnect_ping(int i){
    clients[i].is_working=DISCONNECT;

    int connected_client = clients[i].playing_with;
    if(connected_client !=-1){
        clients[connected_client].playing_with=-1;
        
        if(clients[connected_client].is_working==CONNECT){ 
            unexpected_client_exit(connected_client);  
        }
    }
}


void *pingping(int arg){
    struct message msg1;
    msg1.type=PING;

    while(1){

        for(int i=0;i<MAX_CLIENTS;i++){

            sem_wait(&semaf);
            if(clients[i].is_working==CONNECT){

                if(clients[i].ping==0){
                    clients[i].ping=1;
                    if(sendto(clients[i].socket_type, &msg1, msg_size,0,&clients[i].desc, sizeof(clients[i].desc))==-1){
                        if(errno==ECONNREFUSED)
                            disconnect_ping(i);
                        else 
                            perror("server: sendto ping error");
                    }
                }
                else{
                    disconnect_ping(i);
                }
            }
            sem_post(&semaf);
            sleep(1);
        }   
    }
}




//================================== m a i n ==================

int main(int argc, char ** argv){
    if(argc<3){
        printf("server: wrong number of arguments\n");
        return 1;
    }
    
    signal(SIGINT,sig_handler);
    atexit(exit_handler);

    // sockety ===========================

    strcpy(path,argv[2]);
    int portno;
    sscanf(argv[1], "%d", &portno);
    struct sockaddr_in netaddr;
    netaddr.sin_family = AF_INET;
    netaddr.sin_port = htons(portno);
    struct in_addr inaddr;
    inet_pton(AF_INET,"127.0.0.1",&inaddr);
    netaddr.sin_addr = inaddr;


    struct sockaddr_un unixaddr;
    unixaddr.sun_family=AF_UNIX;
    strcpy(unixaddr.sun_path,argv[2]);

    netdesc = socket(AF_INET, SOCK_DGRAM ,0);
    if(netdesc ==-1)
        perror("server: make socket inet");

    undesc = socket(AF_UNIX, SOCK_DGRAM ,0);
    if(undesc==-1)
        perror("server: make socket unix");


    if(bind(netdesc, (struct sockaddr *) &netaddr, sizeof(netaddr))==-1)
        perror("server: bind socket inet");

    if(bind(undesc, (struct sockaddr *) &unixaddr, sizeof(unixaddr))==-1)
        perror("server: bind socket unix");



    // epoll =============================

    epoldesc = epoll_create1(0);
    if(epoldesc==-1)
        perror("server: epol create error");
    

    struct epoll_event epoll_ev,epoll_ev2;
    epoll_ev.events=EPOLLIN ;
    epoll_ev2.events=EPOLLIN ;
    union epoll_data epoll_da,epoll_da2;
    epoll_da.fd=undesc;
    epoll_ev.data=epoll_da;
    
    if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,undesc,&epoll_ev)==-1)
        perror("server: epoll unix ctl error");

    epoll_da2.fd=netdesc;
    epoll_ev2.data=epoll_da2;
    if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,netdesc,&epoll_ev2)==-1)
        perror("server: epoll net ctl error");


    



// main loop ==========================================================

    clients = (struct client_info *)calloc(MAX_CLIENTS, sizeof(struct client_info));
    struct message received;
    fill=0;

    // pthreads ============================
    sem_init(&semaf,0,1);
    pthread_create(&pthread_id,NULL,(void *)pingping,0);
    pthread_detach(pthread_id);


    struct sockaddr sock_addr;
    socklen_t sock_len = sizeof(sock_addr);
    struct epoll_event event;

    while(1){
        epoll_wait(epoldesc,&event,1,-1);

        recvfrom(event.data.fd,&received, msg_size, MSG_DONTWAIT,&sock_addr,&sock_len);
        
        if(received.type==INIT){
            
            struct client_info new_client;
            struct message msg2,msg;
            strcpy(new_client.name,"");
            new_client.desc=sock_addr;
            new_client.is_working=CONNECT;
            new_client.playing_with=-1;
            new_client.ping=0;
            strcpy(new_client.name,received.name);
            new_client.socket_type=event.data.fd;

            sem_wait(&semaf);

            if(find_client_using_name(received.name) == ERROR){

                 int second_client = find_free_client(new_client.name);

                if(add_client(new_client)==ERROR){
                    printf("cannot add client - array full\n");
                    msg2.msg=ERROR;
                    msg2.type=GIVE_NAME;
                    if(sendto(new_client.socket_type,&msg2, msg_size,0,&new_client.desc, sizeof(new_client.desc))==-1)
                        perror("server: send error arary for player error");
                        
                }                
                else{
                    if(second_client == ERROR){
                        
                        msg.msg=WAITING_FOR_PLAYER;
                        msg.type=CONNECT;
                        if(sendto(new_client.socket_type,&msg, msg_size,0,&new_client.desc, sizeof(new_client.desc))==-1)
                            perror("server: send waiting for player error");

                    }
                    else{
                        int client_id=find_client_using_name(new_client.name);
                        clients[second_client].playing_with=client_id;
                        clients[client_id].playing_with=second_client;
                        msg.other = rand() % 1;

                        strcpy(msg.name,clients[second_client].name);
                        msg.msg=O;
                        msg.type=CONNECT;
                        if(sendto(clients[client_id].socket_type,&msg, msg_size,0,&clients[client_id].desc, sizeof(clients[client_id].desc))==-1)
                            perror("server: send info 1 error");
                        
                        strcpy(msg.name,clients[client_id].name);
                        msg.msg=X;
                        if(sendto(clients[second_client].socket_type,&msg, msg_size,0,&clients[second_client].desc, sizeof(clients[second_client].desc))==-1)
                            perror("server: send info 2 error");
                    } 
                }  

            }
            else{
                msg.msg=ERROR;
                msg.type=CONNECT;
                if(sendto(new_client.socket_type,&msg, msg_size,0,&new_client.desc, sizeof(new_client.desc))==-1)
                    perror("server: send name in use error");
            }
            received.type=-1;
            
            sem_post(&semaf);    

        }            

    // CONNECT
        else if(received.type==CONNECT){

            sem_wait(&semaf);
            int client_id = find_client_using_desc(sock_addr);
            printf("%d\n",client_id);
            struct message msg;

            if(find_client_using_name(received.name) == ERROR){

                strcpy(clients[client_id].name,received.name);
                int second_client = find_free_client(received.name);

                if(second_client == ERROR){
                    
                    msg.msg=WAITING_FOR_PLAYER;
                    msg.type=CONNECT;
                    if(sendto(clients[client_id].socket_type,&msg, msg_size,0,&clients[client_id].desc, sizeof(clients[client_id].desc))==-1)
                        perror("server: send waiting for player error");

                }
                else{
                connect_procedure(received.name,client_id,second_client);    
                }              

            }
            else{
                msg.msg=ERROR;
                msg.type=CONNECT;
                if(sendto(clients[client_id].socket_type,&msg, msg_size,0,&clients[client_id].desc, sizeof(clients[client_id].desc))==-1)
                    perror("server: send name in use error");
            }
            received.type=-1;
            sem_post(&semaf);
                

        }
// DISCONNECT        
        else if(received.type==DISCONNECT){
            sem_wait(&semaf);
            int from_client = find_client_using_name(received.name);
            clients[from_client].is_working=DISCONNECT;


            int connected_client = clients[from_client].playing_with;
            if(connected_client !=-1){
                clients[connected_client].playing_with=-1;
                
                if(received.other==CTRLC && clients[connected_client].is_working==CONNECT){
                    unexpected_client_exit(connected_client);  
                }
            }

            sem_post(&semaf);
            received.type=-1;

        }
// MOVE        
        else if(received.type==MOVE){
            
            sem_wait(&semaf);
            int from_client = find_client_using_name(received.name);
            if(from_client==-1)
                printf("server: find name error\n");

            int to_client = clients[from_client].playing_with;
            if(to_client!=-1){
            
                struct message msg;
                msg.type=MOVE;
                msg.msg=received.msg;
                msg.other=received.other;
                if(sendto(clients[to_client].socket_type,&msg, msg_size,0,&clients[to_client].desc, sizeof(clients[to_client].desc))==-1)
                    perror("server: send move error");
            }
            sem_post(&semaf);
            received.type=-1;

        }
        else if(received.type==PING){
            sem_wait(&semaf);
            int from_client = find_client_using_name(received.name);
            clients[from_client].ping=0;
            sem_post(&semaf);
        }
    } 
}    
