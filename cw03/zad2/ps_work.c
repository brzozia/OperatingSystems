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

struct files{
    char *a;
    char *b;
    char *wy;
};

struct b_part{
    int works;
    int from;   //id of first processed col
    int to;     //id of col after last processed col
};

struct files read_files(char *line){
    int i=0;
    struct files files;
    files.a=(char*)calloc(20,sizeof(char));
    files.b=(char*)calloc(20,sizeof(char));
    files.wy=(char*)calloc(20,sizeof(char));
    strcpy(files.a,"");
    strcpy(files.b,"");
    strcpy(files.wy,"");

    while(line[i]!=' ')i++;
    strncat(files.a,line,i-1);
    while(line[i]!=' ')i++;
    strncat(files.b,line,i-1);
    while(line[i]!='\0')i++;
    strncat(files.wy,line,i-1);

    return files;
}

struct b_part find_part(int col, int ps_no, int i){
    struct b_part bp;
    int step = round(col/ps_no);
    if(step==0 && col != 0) step=1;
    int sum = (i+1)*step;

    if( i<ps_no -1 && sum <col){
        bp.from=i*step;
        bp.to=i*step+step;
        bp.works=1;
    }
    else if(i=ps_no-1){
        bp.from=i*step;
        bp.to=col;
        bp.works=1;
    }
    else
        bp.works=0;

    return bp;
}

int count_col(FILE *bptr){              ///to do
    char *line = (char *)calloc(255,sizeof(char));
    fgets(line,255,bptr);
    if(line==NULL){
        printf("File b is empty");
        return 0;
    }

    int c=1;
    while(line[c]!='\0'){
        if(line[c]==' ')
            c++;
    }
    return c;

}

int mltplc(FILE *a, FILE *b, FILE *wy, struct b_part bp, int w_method ){ ///to do



}

int main(int argc, char ** argv){

    int  mltp_co=0;

    FILE *list=fopen(argv[1], "r");

    if( list == NULL ){
       perror("cannot open file");
       exit(-1);
    }


    char *line = (char *)calloc(255,sizeof(char));

    while(fgets(line,255,list)!=NULL){
        struct files files = read_files(line);                  //reads files names from line of list
        FILE *aptr=fopen(files.a, "r");
        FILE *bptr=fopen(files.b, "r");
        FILE *wyptr=fopen(files.wy, "a");

        if( aptr == NULL || bptr == NULL || wyptr == NULL){
           perror("cannot open file (ps_work)");
           exit(-1);
        }

        struct b_part bp = find_part(count_col(bptr), atoi(argv[2]), atoi(argv[5]));
        rewind(bptr);

        mltp_co+=mltplc(aptr,bptr,wyptr,bp, atoi(argv[4]));        //multiplication of matrices


        fclose(aptr); fclose(bptr); fclose(wyptr);
        free(files.a);free(files.b);free(files.wy);
    }


    fclose(list);


}
