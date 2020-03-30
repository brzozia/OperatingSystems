#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
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
    if(argc<2){
        printf("Wrong given arguments.\n");
        return 1;
    }

    int pending=0;
    if(strcmp(argv[1],"pending")==0)
        pending=1;

    int handler=0;
    if(strcmp(argv[1],"handler")==0)
        handler=1;

    
    if(strcmp(argv[1],"ignore")==0){

        signal(SIGUSR1, SIG_IGN);
    }
    else if(strcmp(argv[1],"handler")==0){

        signal(SIGUSR1, hand);
    }
    else if(strcmp(argv[1],"mask")==0){

        sigset_t new_set;
        sigemptyset(&new_set);
        sigaddset(&new_set,SIGUSR1);

        if(sigprocmask(SIG_SETMASK, &new_set,NULL)<0)
            perror("Unable to block signal\n");

    }
    else if(pending){

        sigset_t new_set;
        sigemptyset(&new_set);
        sigaddset(&new_set,SIGUSR1);

        if(sigprocmask(SIG_SETMASK, &new_set,NULL)<0)
            perror("Unable to block signal\n");

        hand_pending(SIGUSR1);

    }
    else{
        printf("Wrong name of operation.\n");
        return 1;
    }


//first part - fork test
    raise(SIGUSR1);

    if(pending)
        hand_pending(SIGUSR1);

    int child_pid=fork();
    if(child_pid==0){
        printf("I am a child process now:\n");
        if(!pending){
            if(!handler) signal(SIGUSR1, hand);
            raise(SIGUSR1);
        }
        else
            hand_pending(SIGUSR1);
        
        printf("End of child process\n");
        exit(0);
    }
    wait(NULL); 
    

//second part - exec test

    execl("./exec_test","exec_test",argv[1],NULL);
    
}