#include "common.h"

struct client_info{
    char name[32];
    int desc;
    int playing_with;
    int is_working;
};

struct client_info *clients;
int game[3][3];
#define MAX_CLIENTS 14
#define UNIX_PATH_MAX  108


int add_client(struct client_info client){

    for(int i=0;i<MAX_CLIENTS;i++){
        if(clients[i].is_working!=1){
            clients[i]=client;
            return 0;
        }
    }
    return -1;
}

void clear_game(){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
            game[i][j]=-1;
    }
}

void make_move(int cell, int sign){
    
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



    // main loop ========================

    clients = (struct client_info *)calloc(MAX_CLIENTS, sizeof(struct client_info));
    struct message received;


    while(1){
        int read_events = epoll_wait(epoldesc,&epoll_ev,1,-1);
        printf("%d",read_events);

        int read_size = recv(epoldesc,&received, sizeof(struct message),0);

        if(received.msg==CONNECT){
            //connect with other player and check name

        }
        else if(received.msg==MOVE){
            //make move


        }
        else if(received.msg==DISCONNECT){
            //disconnect
        }
        else{
            //connect with server
            int client_desc = accept(epoldesc,NULL,NULL); //????????????
            if(client_desc == -1)
                perror("server: accept connection error");
            
            struct client_info new_client;
            new_client.desc=client_desc;
            new_client.is_working=1;
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