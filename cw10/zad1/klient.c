#include "common.h"

int make_socket_net(char *arg){
    int portno;
    sscanf(arg, "%d", &portno);
    struct sockaddr_in netaddr;
    netaddr.sin_family = AF_INET;
    netaddr.sin_port = htons(portno);

    int netdesc = socket(AF_INET, SOCK_STREAM,0);
    if(netdesc ==-1)
        perror("client: make socket inet");
    
    int connect_status = connect(netdesc, (struct sockaddr *)&netaddr, sizeof(netaddr));
    if(connect_status == -1)
    perror("client: connect network error");

    return netdesc;
}

int make_socket_local(char *arg){
    struct sockaddr_un unixaddr;
    unixaddr.sun_family=AF_UNIX;
    strcpy(unixaddr.sun_path,arg);

    int undesc = socket(AF_UNIX, SOCK_STREAM,0);
    if(undesc==-1)
        perror("client: make socket unix");
    
    int connect_status = connect(undesc, (struct sockaddr *)&unixaddr, sizeof(unixaddr));
    if(connect_status == -1)
    perror("client: connect local error");
    
    return undesc;
}







int main(int argc, char ** argv){
    if(argc<4){
        printf("client: wrong number of arguments\n");
        return 1;
    }

    char name[32];
    strcpy(name,argv[1]);
    int serverdesc;

    if(strcmp(argv[2],"network")==0){
        
        int serverdesc = make_socket_net(argv[1]);
    }
    else if(strcmp(argv[2], "local")==0){
        
        int serverdesc = make_socket_local(argv[2]);
    }
    else{
        perror("client: wrong connection type");
    }

}   