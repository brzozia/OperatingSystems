#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int do_while=1;

void ctrlc(int sig_no){
    printf("\nOdebrano sygnal SIGINT\n");
    exit(0);
}

void ctrlz(int sig_no){

        if(do_while){
            do_while=0;
            printf("\nOczekuje na CTRL-Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
        }else
            do_while=1;
}


int main(){

    char dest[256];
    getcwd(dest,256);
    DIR *dir = opendir(dest);

    if(dir==NULL){
        printf("Cannot open the directory.");
        return 1;
    }

    struct dirent *elem=(struct dirent *)calloc(1,sizeof(struct dirent));    
    char name[312];
    
    struct sigaction sigact;
    sigact.sa_handler=ctrlz;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags=0;

    sigaction(SIGTSTP,&sigact,NULL);
    signal(SIGINT,ctrlc);
    

    while(1){
        if(!do_while)
            pause();

        elem=readdir(dir);
        if(elem==NULL)
            rewinddir(dir);
        else{
            strcpy(name,elem->d_name);
            printf("%s\n",name);            
        } 
        sleep(1);       
    }
        

    closedir(dir);
    free(elem);
}