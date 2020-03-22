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

int main(int argc, char ** argv){

    if(argc<5){
        printf("Wrong given arguments");
        return 1;
    }


    for(int i=0;i<atoi(argv[2]);i++){

        int child_pid=(int)fork();

        if(child_pid==0)
            execl("./ps_work","./ps_work",argv[1],argv[2],argv[3],argv[4],i);
    }

}
