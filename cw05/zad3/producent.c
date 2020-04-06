#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>



int main(int argc, char **argv){

    if(argc<4){
        printf("Wrong given arguments\n");
        return 1;
    }

    int n=atoi(argv[3]);
    FILE *filep=fopen(argv[2], "r");
    if(filep==NULL){
        perror("Cannot open file");
        return 1;
    }
    FILE *fifo=fopen(argv[1],"w");
    if(fifo==NULL){
        perror("Cannot open file");
        return 1;
    }

    char *line=(char*)calloc(n,sizeof(char));

    while(!feof(filep) && n!=0){ 
        strcpy(line,"");
        sleep(rand()%3);

        n=fread(line,sizeof(char),n, filep);
        line[n]='\0';
        fprintf(fifo,"#%d#%s",(int)getpid(),line);
        fflush(fifo);
        printf("prod %s\n",line);
    }


    fclose(fifo);
    fclose(filep);
    free(line);
    exit(0);
}