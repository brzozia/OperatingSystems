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

void hand(int sig_no){
    printf("I am in handler\n");
}

void sighand(int sig, siginfo_t *info, void *ucontext){
    printf("\nI am %d and I am in sa_sigaction handler\n",sig);
    if(sig==SIGCHLD)
        printf("Status of child process: %d\nReal user ID %d\nCode of signal %d\n",info->si_status,info->si_uid, info->si_code );
   else
        printf("Address of fault: %p\nCode of signal %d\nErrno: %d\n",info->si_addr, info->si_code,info->si_errno );
   
   if(sig==SIGINT)
   exit(0);
}

int main(){

    struct sigaction sigact;
    sigact.sa_handler=hand;
    sigact.sa_sigaction=sighand;
    sigemptyset(&sigact.sa_mask);

    sigact.sa_flags=SA_SIGINFO;

    sigaction(SIGUSR1,&sigact,NULL);
    sigaction(SIGCHLD,&sigact,NULL);
    sigaction(SIGFPE,&sigact,NULL);
    sigaction(SIGALRM,&sigact,NULL);
    sigaction(SIGINT,&sigact,NULL);
    

    raise(SIGUSR1);
    raise(SIGFPE);
    raise(SIGALRM);


    int child_pid=fork();
    if(child_pid==0){
    int a=2147483640,b=22;
        b=a+b;
    }
    wait(NULL);

    raise(SIGINT);

    return 0;
}