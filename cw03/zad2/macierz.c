#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "ps_work.c"

int main(int argc, char ** argv){

    if(argc<5){
        printf("Wrong given arguments");
        return 1;
    }

    int child_pid, proc=atoi(argv[2]), PPID=(int)getpid(), w_method=atoi(argv[4]);
    struct rlimit *time=(struct rlimit*)calloc(1,sizeof(struct rlimit));



    FILE *mlt_tasks=fopen("tasks.tmp","w+");
    FILE *list=fopen(argv[1], "r");

    if( list == NULL || mlt_tasks==NULL){
       perror("cannot open file");
       exit(-1);
    }

    char line[255];
    char **wy_files = (char**)calloc(255,sizeof(char*));
    int wfile=0;


    //reads files names from line of list
    while(fgets(line,255,list)!=NULL){                          //to make list of tasks for processes to do

        struct files filee= read_files(line);
        wy_files[wfile]=(char*)calloc(20,sizeof(char));
        strcpy(wy_files[wfile],filee.wy);
        FILE *bptr=fopen(filee.b, "r");
        FILE *aptr=fopen(filee.a, "r");
        FILE *wyptr=fopen(filee.wy, "w");

        if( bptr == NULL || aptr==NULL || wyptr==NULL){
           perror("cannot open file ");
           exit(-1);
        }

        int b_col = count_col(bptr);
        for(int i=0;i<proc;i++){

            struct b_part bp = find_part(b_col, proc, i);
                rewind(bptr);
            fprintf(mlt_tasks,"%d %s %s %d %d %d %s\n",i,filee.a,filee.b,bp.from,bp.to,bp.works,filee.wy);
        }


        //prepare places for results in result-files
        int a_row = count_row(aptr);
        char *wy_line=(char*)calloc(100,sizeof(char));
        rewind(wyptr);
        while(b_col>0){
            strcat(wy_line,"- ");
            b_col--;
        }
        while(a_row>0){
            fprintf(wyptr,"%s\n",wy_line);
            a_row--;
        }
        fflush(wyptr);

        fclose(bptr);fclose(aptr);fclose(wyptr);
        free(filee.a);free(filee.b);free(filee.wy);free(wy_line);
        wfile++;
    }
    fclose(list);fclose(mlt_tasks);


     //makes processes
    for(int i=0;i<proc;i++){

        if((int)getpid()==PPID) child_pid=(int)fork();

       if(child_pid==0 ){
            getrlimit(RLIMIT_CPU,time);
            if(time->rlim_max<=atoi(argv[3])) time->rlim_cur=atoi(argv[3]);

            ps_work(argv[1],proc,atof(argv[3]),w_method,i);
        }
    }

    int status;
    while((child_pid=wait(&status))!=-1)
            printf("Process %d made %d multiplications\n", (int)child_pid, WEXITSTATUS(status));





    int wfile2=wfile;
    if(w_method==1){

        for(int i=0;i<wfile ;i++)
            free(wy_files[i]);
        free(wy_files);

        return 0;
    }

    else{

        wfile--;
        char *wy_name=(char*)calloc(20,sizeof(char));
        char name[100];

        while(wfile>=0){

            wy_name=wy_files[wfile];
            strcpy(name,"paste ");
             //for(int i=1;i<proc+1;i++){
                 //sprintf(tostr,"%d",i-1);
                 strcat(name,wy_name);
                 //strcat(name,tostr);
                 strcat(name, "* ");
            // }

            strcat(name,"-d ");
            strcat(name, "\" \" > ");
            strcat(name, wy_name);

                int cpid=fork();
                if(cpid==0){
                    execl("./pastef","pastef",name,NULL);
                }
                wait(NULL);

            wfile--;
        }

        for(int i=0;i<wfile2;i++)
            free(wy_files[i]);
        free(wy_files);
    }

    return 0;


}
