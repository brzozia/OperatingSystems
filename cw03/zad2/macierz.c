#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/resource.h>
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

    int child_pid, proc=atoi(argv[2]), PPID=(int)getpid(), w_method=atoi(argv[4]);
    struct rlimit *time=(struct rlimit*)calloc(1,sizeof(struct rlimit));

    for(int i=0;i<proc;i++){

        if((int)getpid()==PPID) child_pid=(int)fork();

        if(child_pid==0 ){
            printf("ACT PID: %d, PAR PID: %d\n",(int)getpid(),(int)getppid());
            getrlimit(RLIMIT_CPU,time);
            if(time->rlim_max<=atoi(argv[3])) time->rlim_cur=atoi(argv[3]);
            ps_work(argv[1],proc,w_method,i);
        }
    }

    int status;
    while((child_pid=wait(&status))!=-1)
            printf("Proces %d wykonal %d mnozen\n", (int)child_pid, WEXITSTATUS(status));



    //if(w_method==1)
    return 0;
/*

    char *line = (char *)calloc(255,sizeof(char));
    FILE *list = fopen(argv[2],"r"); rewind(list);
    int i=0;
    while(fgets(line,255,list)!=NULL){

        struct files f=read_files(line);
        FILE *b=fopen(f.b,"r");
        int col=count_col(b);
        int step=round(col/proc); if(step==0 && col!=0) step=1; int s=0;
        FILE *wy=fopen(f.wy,"w");
        FILE *res=fopen("res.txt","r");

        char *filen=(char*)calloc(10000,sizeof(char));
        char *fname=(char*)calloc(20,sizeof(char));
        fgets(filen,10000,res);
        int p=0,s=0;
        while(filen[p]!='\0'){
            if(filen[p]==';'){
                strncpy(fname,filen+s,p-s);
                s=p;
                int pid=fork();
                if(pid==0)
                    exec("paste",)
            }
            else
                p++;

        }


        fclose(wy);
        fclose(res);
        free(line);free(filen);
        i++;
    }
    */
}
