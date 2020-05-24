#include "common.h"

int game[3][3];
int place=0;
char name[32];
int my_sign;
int not_my_sign;
int serverdesc;
int epoldesc;
char path[248];


int make_socket_net(char *arg){
    int portno;
    sscanf(arg, "%d", &portno);
    struct sockaddr_in netaddr;
    netaddr.sin_family = AF_INET;
    netaddr.sin_port = htons(portno);
    struct in_addr inaddr;
    inet_pton(AF_INET,"127.0.0.1",&inaddr);
    netaddr.sin_addr = inaddr;

    
    int netdesc = socket(AF_INET, SOCK_DGRAM ,0);
    if(netdesc ==-1)
        perror("client: make socket inet");

    int connect_status = connect(netdesc, (struct sockaddr *)&netaddr, sizeof(netaddr));
    if(connect_status == -1)
    perror("client: connect network error");

    return netdesc;
}

int make_socket_local(char *arg){
    strcpy(path,arg);

    struct sockaddr_un unixaddr;
    unixaddr.sun_family=AF_UNIX;
    strcpy(unixaddr.sun_path,path);

    int undesc = socket(AF_UNIX, SOCK_DGRAM ,0);
    if(undesc==-1)
        perror("client: make socket unix");
    
    if(bind(undesc, (struct sockaddr *) &unixaddr, sizeof(sa_family_t))==-1)
        perror("client: bind socket unix");
    
    int connect_status = connect(undesc, (struct sockaddr *)&unixaddr, sizeof(unixaddr));
    if(connect_status == -1)
    perror("client: connect local error");
    
    return undesc;
}

/////////////////////


void print_game(){
    for(int i=0;i<3;i++){
        char line[7];
        int p=0;
        for(int j=0;j<3;j++){
            int sign = game[i][j];

            if(sign==FREE)
                line[p]='_';
            else if(sign==X)
                line[p]='X';
            else 
                line[p]='O';

            line[++p]=' ';
            p++;
        }
        printf("%s\n",line);
    }
}


int check_winner(){
    
    for(int i=0;i<3;i++){
        int suma=game[0][i]+game[1][i]+game[2][i];
        int suma2=game[i][0]+game[i][1]+game[i][2];
        if(suma==3*my_sign || suma2==3*my_sign)
            return WINNER;
    }
    if(game[0][0]+game[1][1]+game[2][2]==3*my_sign)
        return WINNER;
    if(game[0][2]+game[1][1]+game[2][0]==3*my_sign)
        return WINNER;
    
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(game[i][j]==FREE)
                return NOT_WINNER;
        }
    }

    return REMIS; 
}

void disconnect(){
    printf("disconnect with server\n");
    
    // if(shutdown(serverdesc, SHUT_RDWR)==-1)
    //     perror("server: shutdown socket error");

    // if(close(serverdesc)==-1)
    //     perror("server: close socket error");

    exit(0);
}

void clear_game(){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++)
            game[i][j]=FREE;
    }
}

int write_move(int cell, int sign){
    if(cell==ERROR)
        return 0;

    int col=-1,row=0;

    while(cell>0){
        col++;
        cell--;

        if(col==3){
            col=0;
            row++;
        }
    }
    
    if(game[row][col]!=FREE)
        return ERROR;
    
    game[row][col]=sign;
    place++;
    return 0;
}


void ping_fun(){
    struct message msg4;
    msg4.type=PING;
    strcpy(msg4.name,name);
    write(serverdesc,&msg4,msg_size);
}

void disconnect_fun(){
    struct message msg3;
    msg3.type=DISCONNECT;
    strcpy(msg3.name,name);
    write(serverdesc,&msg3,msg_size);
    disconnect();
}

void do_move(int serverdesc){
    print_game();
    printf("Your move. Choose number from 1 to 9.\n");

    int move;
    struct message msg,received;
    struct epoll_event event;


    while(1){
        epoll_wait(epoldesc,&event,1,-1);

        if(event.data.fd==serverdesc){

            read(serverdesc,&received,msg_size);
            if(received.type==PING){
               ping_fun();
            }
            else if(received.type==DISCONNECT){
                disconnect_fun();
            }
            else if(received.type==CONNECT && received.msg==WAITING_FOR_PLAYER){
                if(received.other==ERROR){
                    printf("Second player has left the game.\n");
                }
                printf("Waiting for other player...\n");
                return;
            }
        }
        if(event.data.fd==STDIN_FILENO)
            break;
    }


    scanf("%d",&move);
    

    if(move>9 || move <1 || write_move(move,my_sign) == ERROR){
        printf("Wrong cell number. You have lost your turn\n");
        move = ERROR;
    }
    print_game();
    place++;

    strcpy(msg.name,name);
    msg.type=MOVE;
    msg.msg=move;

    int result=check_winner();
    if(result==WINNER){
        printf("CONGRATULATIONS! YOU WON!\n");
        msg.other=WINNER;
        write(serverdesc,&msg,sizeof(struct message));

        msg.type=DISCONNECT;
        write(serverdesc,&msg,sizeof(struct message));

        disconnect();
    }
    else if(result==REMIS){
        printf("CONGRATULATIONS! REMIS!\n");
        msg.other=REMIS;
        write(serverdesc,&msg,sizeof(struct message));

        msg.type=DISCONNECT;
        write(serverdesc,&msg,sizeof(struct message));

        disconnect();
    }
    else{
         write(serverdesc,&msg,sizeof(struct message));     
    }
}


void sig_handler(int signum){
    struct message msg;
    strcpy(msg.name,name);
    msg.type=DISCONNECT;
    msg.other=CTRLC;
    write(serverdesc,&msg,sizeof(struct message));

    disconnect();
}


//                  m a i n 

int main(int argc, char ** argv){
    if(argc<4){
        printf("client: wrong number of arguments\n");
        return 1;
    }

    signal(SIGINT,sig_handler);

    strcpy(name,argv[1]);

    if(strcmp(argv[2],"network")==0){
        serverdesc = make_socket_net(argv[3]);
    }
    else if(strcmp(argv[2], "local")==0){
        serverdesc = make_socket_local(argv[3]);
    }
    else{
        perror("client: wrong connection type");
        return 1;
    }

    epoldesc = epoll_create1(0);
    if(epoldesc==-1)
        perror("server: epol create error");
    

    struct epoll_event epoll_ev,epoll_ev2;
    epoll_ev.events=EPOLLIN ;
    epoll_ev2.events=EPOLLIN ;
    union epoll_data epoll_da,epoll_da2;
    epoll_da.fd=serverdesc;
    epoll_ev.data=epoll_da;
    
    if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,serverdesc,&epoll_ev)==-1)
        perror("server: epoll unix ctl error");

    epoll_da2.fd=STDIN_FILENO;
    epoll_ev2.data=epoll_da2;
    if(epoll_ctl(epoldesc,EPOLL_CTL_ADD,STDIN_FILENO,&epoll_ev2)==-1)
        perror("server: epoll net ctl error");

    struct message received;
    received.type=-1;

    struct message msg;
    strcpy(msg.name,argv[1]);
    msg.type=INIT;

    int send_status = write(serverdesc,&msg,msg_size);
    if(send_status==-1)
        perror("client: send name error");


//============================================================
    while(1){
        int rcv_status = read(serverdesc,&received,msg_size);

        if(rcv_status==0){
            disconnect();
        }

        if(received.type==GIVE_NAME){
            if(received.msg==ERROR){
                printf("Cannot add player. Try again later.\n");
                exit(-1);
            }
            struct message msg;
            strcpy(msg.name,argv[1]);
            msg.type=CONNECT;

            int send_status = write(serverdesc,&msg,msg_size);
            if(send_status==-1)
                perror("client: send name error");
            received.type=-1;
        }
        else if(received.type==CONNECT){

            if(rcv_status==-1)
                perror("client: receive msg error");
            else{

                if(received.msg==ERROR){
                    printf("Player with this name is already connected.\n");
                    exit(-1);
                }
                else if(received.msg==WAITING_FOR_PLAYER){
                    if(received.other==ERROR){
                        printf("Second player has left the game.\n");
                    }
                    printf("Waiting for other player...\n");
                }
                else{
                    my_sign=received.msg;
                    
                    char s;
                    if(my_sign==O){
                        s='O';
                        not_my_sign=X;
                    }
                    else{
                        s='X';
                        not_my_sign=O;
                    }

                    printf("Connected with player %s. Your sign is %c.\n",received.name,s);
                    clear_game();
                    if(received.other==my_sign){
                        printf("You have to start.\n");
                        do_move(serverdesc);
                    }
                    
                }
                received.type=-1;
            }
        }
        else if(received.type==MOVE){

            write_move(received.msg,not_my_sign);
            printf("After second player move:\n");

            if(received.other==WINNER){
                print_game();
                printf("YOU LOST. TRY AGAIN.\n");
                struct message msg;
                strcpy(msg.name,name);
                msg.type=DISCONNECT;
                write(serverdesc,&msg,msg_size);
                disconnect();
            }
            else if(received.other==REMIS){
                print_game();
                printf("CONGRATULATIONS! REMIS!\n");
                struct message msg;
                strcpy(msg.name,name);
                msg.type=DISCONNECT;
                write(serverdesc,&msg,msg_size);
                disconnect();
            }
            else         
                do_move(serverdesc);
        }
        else if(received.type==DISCONNECT){
            disconnect_fun();
        }
        else if(received.type==PING){
            ping_fun();
        }
    }
}   

