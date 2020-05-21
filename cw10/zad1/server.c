#include "common.h"

struct client_info{
    char name[32];
    int desc;
    int playing_with;
    int is_working;
    int sign;
};

struct client_info *clients;
struct game *games;

#define MAX_CLIENTS 14
#define UNIX_PATH_MAX  108




int add_client(struct client_info client){

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
        if(strcmp(name, clients[i].name)==0)
            return i;
    }
    return ERROR;
}


int find_client_using_desc(int desc){
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].desc==desc)
        return i;
    }
    return ERROR;
}

int find_free_client(int desc){
    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].is_working==CONNECT && clients[i].playing_with==-1 && clients[i].desc!=desc)
        return i;
    }
    return ERROR;
}

//================================== m a i n ==================

int main(int argc, char ** argv){
    if(argc<3){
        printf("server: wrong number of arguments\n");
        return 1;
    }
    
    // sockety ===========================

    int portno;
    sscanf(argv[1], "%d", &portno);
    struct sockaddr_in netaddr;
    netaddr.sin_family = AF_INET;
    netaddr.sin_port = htons(portno);

    struct sockaddr_un unixaddr;
    unixaddr.sun_family=AF_UNIX;
    strcpy(unixaddr.sun_path,argv[2]);

    int netdesc = socket(AF_INET, SOCK_STREAM,0);
    if(netdesc ==-1)
        perror("server: make socket inet");

    int undesc = socket(AF_UNIX, SOCK_STREAM,0);
    if(undesc==-1)
        perror("server: make socket unix");


    if(bind(netdesc, (struct sockaddr *) &netaddr, sizeof(netaddr))==-1)
        perror("server: bind socket inet");

    if(bind(undesc, (struct sockaddr *) &unixaddr, sizeof(unixaddr))==-1)
        perror("server: bind socket unix");


    listen(netdesc,MAX_CLIENTS);
    listen(netdesc,MAX_CLIENTS);

    // epoll =============================

    int epoldesc = epoll_create1(0);
    if(epoldesc==-1)
        perror("server: epol create error");
    

    struct epoll_event epoll_ev;
    epoll_ev.events=EPOLLIN | EPOLLOUT;
    union epoll_data epoll_da;
    epoll_da.fd=undesc;
    epoll_ev.data=epoll_da;
    
    if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,undesc,&epoll_ev)==-1)
        perror("server: epoll unix ctl error");

    epoll_da.fd=netdesc;
    if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,netdesc,&epoll_ev)==-1)
        perror("server: epoll net ctl error");




// main loop ==========================================================

    clients = (struct client_info *)calloc(MAX_CLIENTS, sizeof(struct client_info));
    struct message received;



    while(1){
        int read_events = epoll_wait(epoldesc,&epoll_ev,1,-1);
        printf("%d",read_events);

        int read_size = recv(epoldesc,&received, sizeof(struct message),0);

// CONNECT
        if(received.type==CONNECT){

            int client_id = find_client_using_desc(epoll_ev.data.fd);
            if(find_client_using_name(received.name) == ERROR){
                strcpy(clients[client_id].name,received.name);

                int second_client = find_free_client(epoll_ev.data.fd);
                struct message msg;

                if(second_client == ERROR){

                    msg.msg=WAITING_FOR_PLAYER;
                    msg.type=CONNECT;
                    if(write(clients[client_id].desc,&msg, sizeof(struct message))==-1)
                        perror("server: send waiting for player error");

                }
                else{
                    
                    clients[second_client].playing_with=client_id;
                    clients[client_id].playing_with=second_client;

                    strcpy(msg.name,clients[second_client].name);
                    msg.msg=O;
                    msg.type=CONNECT;
                    if(write(clients[client_id].desc,&msg, sizeof(struct message))==-1)
                        perror("server: send info 1 error");
                    
                    strcpy(msg.name,clients[client_id].name);
                    msg.msg=X;
                    msg.type=CONNECT;
                    if(write(clients[second_client].desc,&msg, sizeof(struct message))==-1)
                        perror("server: send info 2 error");
                }   

            }
            else{
                msg.msg=ERROR;
                msg.type=CONNECT;
                if(write(clients[client_id].desc,&msg, sizeof(struct message))==-1)
                    perror("server: send name in use error");
            }
                

        }
// MOVE        
        else if(received.type==MOVE){

            int from_client = find_client_using_name(received.name);
            int to_client = clients[from_client].playing_with;
            struct message msg;
            msg.type=MOVE;
            msg.msg=received.msg;
            if(write(clients[to_client].desc,&msg, sizeof(struct message))==-1)
                perror("server: send move error");

        }
// DISCONNECT        
        else if(received.type==DISCONNECT){
            int from_client = find_client_using_name(received.name);
            clients[from_client].is_working=DISCONNECT;
            
            epoll_da.fd=clients[from_client].desc;
            if(epoll_ctl(epoldesc,EPOLL_CTL_DEL,undesc,&epoll_ev)==-1)
                perror("server: epoll unix ctl error");
            
            if(shutdown(clients[from_client].desc, SHUT_RDWR)==-1)
                perror("server: shutdown socket error");

            if(close(clients[from_client].desc)==-1)
                perror("server: close socket error");
        }
// ACCEPT        
        else{
            //connect with server
            int client_desc = accept(epoldesc,NULL,NULL); //????????????
            if(client_desc == -1)
                perror("server: accept connection error");
            
            epoll_da.fd=client_desc;
            if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,client_desc,&epoll_ev)==-1)
                perror("server: epoll net ctl error");
            
            struct client_info new_client;
            new_client.desc=client_desc;
            new_client.is_working=CONNECT;
            new_client.playing_with=-1;
            if(add_client(new_client)==-1)
                printf("cannot add client - array full");
        }
    }







    // closing and cleaning ==============

    epoll_da.fd=undesc;
    epoll_ev.data=epoll_da;
    if(epoll_ctl(epoldesc,EPOLL_CTL_DEL,undesc,&epoll_ev)==-1)
        perror("server: epoll unix ctl error");

    epoll_da.fd=netdesc;
    epoll_ev.data=epoll_da;
    if(epoll_ctl(epoldesc,EPOLL_CTL_DEL,netdesc,&epoll_ev)==-1)
        perror("server: epoll net ctl error");

    close(netdesc);
    close(undesc);
    unlink(argv[2]);
    free(clients);

}