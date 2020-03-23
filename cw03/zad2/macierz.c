#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include "ps_work.c"

int main(int argc, char ** argv){

    if(argc<5){
        printf("Wrong given arguments");
        return 1;
    }

    int child_pid, proc=atoi(argv[2]), PPID=(int)getpid();

    for(int i=0;i<proc;i++){

        if((int)getpid()==PPID) child_pid=(int)fork();

        if(child_pid==0 ){
            printf("ACT PID: %d, PAR PID: %d\n",(int)getpid(),(int)getppid());
            ps_work(argv[1],proc,argv[3],atoi(argv[4]),i);
        }
    }

    int status;
    while((child_pid=wait(&status))!=-1)
        printf("Proces %d wykonal %d mnozen\n", (int)child_pid, (int)status);


    return 0;

}
