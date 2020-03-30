#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

void hand(int sig_no){
    printf("Handler has received signal SIGUSR1\n");
}

void hand_pending(int sig_no){
    sigset_t waiting;
    sigemptyset(&waiting);

    sigpending(&waiting);
    int res=sigismember(&waiting,SIGUSR1);
    if(res==1)
        printf("SIGUSR1 is waiting\n");
    else if(res==0)
        printf("SIGUSR1 is not waiting\n");
    else
        perror("Something went wrong in pending\n"); 

}

int main(int argc, char **argv){

    printf("I'm in exec function\n");
    if(strcmp(argv[1],"pending")==0)
        hand_pending(SIGUSR1);
    
    signal(SIGUSR1,hand);

    if(strcmp(argv[1],"pending")!=0)
        raise(SIGUSR1);
    
    
    exit(0);
}