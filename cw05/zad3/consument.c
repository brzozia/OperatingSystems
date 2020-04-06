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
    FILE *filewr=fopen(argv[2], "w");
    if(filewr==NULL){
        perror("Cannot open file");
        return 1;
    }
    FILE *fifo=fopen(argv[1],"r");
    if(fifo==NULL){
        perror("Cannot open file");
        return 1;
    }

     char *line=(char*)calloc(n,sizeof(char));

    while(!feof(fifo)){ 
        strcpy(line,"");

        n=fread(line,sizeof(char),n, fifo);
        line[n]='\0';
        fprintf(filewr,"%s\n",line);
        fflush(filewr);
        printf("cons %s\n",line);
    }


    fclose(fifo);
    fclose(filewr);
    free(line);
    exit(0);

}