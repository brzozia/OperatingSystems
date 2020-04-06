#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


int maxs=0;

char **find_args(char *command){
    char **args=(char**)calloc(128, sizeof(char*));

    int p=0, it=0, flag=0;
    for(int i=0;command[i]!='\0';i++){
        if((command[i]=='\''|| command[i]=='\"'|| command[i]=='{')&& flag==0)
            flag=1;
        else if((command[i]=='\''  || command[i]=='\"'|| command[i]=='}')&& flag==1)
            flag=0;
        
        
        if(command[i]==' ' && flag==0){

            args[it]=(char*)calloc(i-p+64, sizeof(char));
            strncpy(args[it++], command+p,i-p);
            p=i+1;
        }
        else if(command[i+1]=='\0'){

            args[it]=(char*)calloc(i-p+64, sizeof(char));
            strncpy(args[it++], command+p,i+1-p+1);
        }

    }
    args[it]=NULL;
    
        maxs=it;

    return args;
}


int main (int argc, char ** argv){
    if(argc <2){
        printf("Wrong number of arguments");
        return 1;
    }

    char path[200];
    strcpy(path,argv[1]);
    char oneline[2048];
    FILE *file=fopen(path, "r");
    if(file==NULL){
        perror("cannot open file");
        return(1);
    }

    while(fgets(oneline,2048,file)!=NULL){
    
        //reads how many commands are there
        int n=1;
        for(int i=0;oneline[i]!='\0';i++){
            if(oneline[i]=='|')
                n++;  
        }
        
        //reads commands
        char command[256];
        strcpy(command,"");
        int p=0,end=0;
        char tasks[128][256];

        int i=0;
        for(;oneline[i]!='\0';i++){

            if(oneline[i]=='|' || oneline[i]=='\n'){
                
                strcpy(tasks[p++],command);
                strcpy(command,"");
                i++; end=0;
            }
            else{
                end++;
                strncat(command,oneline+i,1);
                command[end+1]='\0';

                
            }
        }
        if( oneline[i]=='\0'){
                strcpy(tasks[p++],command);
                strcpy(command,"");
        }

        //make pipe
        char **args;
        int fre[2], fwr[2];
        pipe(fre);

        for(int i=0;i<n;i++){
            pipe(fwr);

            if (fork()== 0) {

                if(i!=0){
                    dup2(fre[0],STDIN_FILENO);
                    close(fre[1]);
                }
                if(i!=n-1){
                    close(fwr[0]);
                    dup2(fwr[1],STDOUT_FILENO);
                }

                    args=find_args(tasks[i]);
                    execvp(args[0],args);
                    perror("error");

                    for(int i=0;i<maxs;i++){
                        free(args[i]);
                    }
                    free(args);
                    exit(0);
            } 
            close(fre[0]);
            close(fre[1]);
            fre[0]=fwr[0];
            fre[1]=fwr[1];
        }
        close(fwr[0]);
        close(fwr[1]);

        while((wait(NULL))!=-1);
    }

    fclose(file); 
}