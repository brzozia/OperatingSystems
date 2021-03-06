#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>

int usr1count=0;
char *sendway;

char *lower(char *s){
    char *name=s;
    int size=strlen(s);
    for(int i=0;i<size;i++){
        name[i]=tolower(s[i]);
    }
    return name;
}

void usr1_hand(int sig_no){
    usr1count++;
}

void usr2_hand(int sig, siginfo_t *info, void *ucontext){
    int sendPID=info->si_pid;

    if(strcmp(sendway,"kill")==0){
        for(int i=0;i<usr1count;i++){
            kill(sendPID,SIGUSR1);
        }
        kill(sendPID,SIGUSR2);
    }
    else if(strcmp(sendway,"sigqueue")==0){
        union sigval val;
        for(int i=0;i<usr1count;i++){
            val.sival_int=i;
            sigqueue(sendPID,SIGUSR1,val);
        }
        sigqueue(sendPID,SIGUSR2,val);

    }
    else if(strcmp(sendway,"sigrt")==0){
        for(int i=0;i<usr1count;i++){
            kill(sendPID,SIGRTMIN+1);
        }
        kill(sendPID,SIGRTMIN+2);

    }
    printf("Catcher: Recived signals: %d\n",usr1count);
    exit(0);
}



int main(int argc, char **argv){

    if(argc<2){
        printf("wrong number of arguments\n");
        return 1;
    }
    
    printf("Catcher's PID id: %d\n", (int)getpid());
    sendway=lower(argv[1]);


    sigset_t unblock;
    sigemptyset(&unblock);
    if(strcmp(sendway,"sigrt")==0){
        sigaddset(&unblock,SIGRTMIN+1);
        sigaddset(&unblock,SIGRTMIN+2);
    }
    else{
        sigaddset(&unblock,SIGUSR1);
        sigaddset(&unblock,SIGUSR2);
    }
    if(sigprocmask(SIG_UNBLOCK,&unblock,NULL)<0)
        perror("Cannoc unblock sigals");


    struct sigaction usr1act,usr2act;
    usr1act.sa_handler=usr1_hand;
    usr2act.sa_sigaction=usr2_hand;
    sigemptyset(&usr1act.sa_mask);
    sigemptyset(&usr2act.sa_mask);
    if(strcmp(sendway,"sigrt")==0){
        sigaddset(&usr1act.sa_mask,SIGRTMIN+2);
        sigaddset(&usr2act.sa_mask,SIGRTMIN+1);
    }
    else{
        sigaddset(&usr1act.sa_mask,SIGUSR2);
        sigaddset(&usr2act.sa_mask,SIGUSR1);
    }
    usr1act.sa_flags=0;
    usr2act.sa_flags=SA_SIGINFO;

    if(strcmp(sendway,"sigrt")==0){
        sigaction(SIGRTMIN+1,&usr1act,NULL);
        sigaction(SIGRTMIN+2,&usr2act,NULL);
    }
    else{
        sigaction(SIGUSR1,&usr1act,NULL);
        sigaction(SIGUSR2,&usr2act,NULL);
    }

    sleep(3);

    while(1);

}