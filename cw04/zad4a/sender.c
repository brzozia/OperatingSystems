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

int received=0;
int sended;
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
    received++;
}

void usr2_hand(int sig, siginfo_t *info, void *ucontext){
    printf("Sender: Received %d SIGUSR1 signals from catcher\n",received);
    if(strcmp(sendway,"sigqueue")==0)
        printf("Sender: Should received %d signals from catcher\n",info->si_value.sival_int);
    else
        printf("Sender: Sended to catcher %d signals",sended);
    
    exit(0);
}


//PID-catcher no_of_signals_to_send how_to_send
int main(int argc, char **argv){

    if(argc<4){
        printf("wrong number of arguments\n");
        return 1;
    }

    sended=0;
    sendway=lower(argv[3]);
    int catPID=atoi(argv[1]);
    int signo=atoi(argv[2]);
    if(strcmp(sendway,"kill")==0){
        for(int i=0;i<signo;i++){
            sended++;
            kill(catPID,SIGUSR1);
        }
        kill(catPID,SIGUSR2);
    }
    else if(strcmp(sendway,"sigqueue")==0){
        union sigval val;
        for(int i=0;i<signo;i++){
            sended++;
            val.sival_int=i;
            sigqueue(catPID,SIGUSR1,val);
        }
        sigqueue(catPID,SIGUSR2,val);
    }
    else if(strcmp(sendway,"sigrt")==0){
        for(int i=0;i<signo;i++){
            sended++;
            kill(catPID,SIGRTMIN+1);
        }
        kill(catPID,SIGRTMIN+2);
    }
    else{
        printf("Wrong third argument\n");
        return 1;
    }



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

    while(1);
}