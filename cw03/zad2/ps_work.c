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
#include <sys/file.h>

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

void print_matrix(int **a,int wid, int hei){
    for(int i=0;i<hei;i++){
        for(int j=0;j<wid;j++){
            printf("%d ", a[i][j]);

        }
        printf("\n");
    }

}

struct files read_files(char *line){
    printf("wtf");
    int i=0;
    struct files file;
    file.a=(char*)calloc(20,sizeof(char));
    file.b=(char*)calloc(20,sizeof(char));
    file.wy=(char*)calloc(20,sizeof(char));
    strcpy(file.a,"");
    strcpy(file.b,"");
    strcpy(file.wy,"");

    while(line[i]!=' ')i++;
    strncpy(file.a,line,i);
    i++;
    int n=i;
    printf("a: %s",file.a);
    while(line[n]!=' ')n++;
    strncpy(file.b,line+i,n-i);
    n++;
    i=n;
    printf("b %s", file.b);
    while(line[i]!='\0')i++;
    strncat(file.wy,line+n,i-n-1);
    printf("wy: %s",file.wy);
    return file;
}

struct b_part find_part(int col, int ps_no, int i){
    struct b_part bp;
    printf("kolo\n");
    int step = round(col/ps_no);
    printf("step: %d", step);
    if(step==0 && col != 0) step=1;
    int sum = (i+1)*step;

    if( i<ps_no -1 && sum <col){
        bp.from=i*step;
        bp.to=i*step+step;
        bp.works=1;
    }
    else if(i==ps_no-1){
        bp.from=i*step;
        bp.to=col;
        bp.works=1;
    }
    else
        bp.works=0;

    return bp;
}

int count_col(FILE *bptr){
    rewind(bptr);
    char *line = (char *)calloc(10000,sizeof(char));
    fgets(line,10000,bptr);
    if(line==NULL){
        printf("File b is empty");
        return 0;
    }

    int c=1, i=0;
    while(line[i]!='\0'){
        if(line[i]==' ')
            c++;
        i++;
    }
    rewind(bptr);
    return c;
}

int count_row(FILE *a){
    int hei =0;
    rewind(a);
    char *line = (char *)calloc(10000,sizeof(char));

    while(fgets(line,10000,a)!=NULL) hei++;

    free(line);
    rewind(a);
    return hei;
}

int **make_matrix(FILE *a, int firstc, int lastc){
    int wid = lastc-firstc; // number of cells widely
    int hei =count_row(a);
    char *line = (char *)calloc(10000,sizeof(char));
    rewind(a);

    int **matrix= (int**)calloc(hei,sizeof(int*));


    for(int i=0;i<hei;i++){
        matrix[i]=(int*)calloc(wid,sizeof(int));

        fgets(line,10000,a);

        int j=0,p=0,sp=0;
        while(sp<firstc){        // finding where to start copying numbers when we have only specyfic columns to copy
            if(line[p]==' ')
                sp++;
            p++;
        }
        j=p;
        while(sp<lastc && line[p]!='\0'){        // and where to end
            if(line[p]==' ')
                sp++;
            p++;
        }

        int no=0, no_ctr=0;
        while(j<p){                         //copy elements
             if(line[j]==' ' || line[j]=='\n'){
                matrix[i][no_ctr]=no;
                no_ctr++;
                no=0;
            }
            else if(line[j]!=' '){
                no=no*10 + line[j]-'0';
            }

            j++;
        }
    }

    free(line);
    return matrix;
}

int mltplc(FILE *a, FILE *b, FILE *wy, struct b_part bp, int w_method ){

    int a_col=count_col(a);
    int a_row = count_row(a);
    int wid=bp.to-bp.from;
    int **a_matrix = make_matrix(a, 0, a_col);
    print_matrix(a_matrix,a_col,a_row);

    int **b_matrix = make_matrix(b, bp.from, bp.to);rewind(b);
    print_matrix(b_matrix,wid,count_row(b));

    int result[a_row][wid];
    int num=0, p=0;

    while(p<wid){
        for(int j=0;j<a_row;j++){
            for(int i=0;i<a_col;i++){
                num=num + (a_matrix[j][i])*(b_matrix[i][p]);
            }
        result[j][p]=num;
        num=0;
        }
        p++;
    }


    if(w_method==1){
    printf("aaaa");
        int fd=fileno(wy);
        flock(fd,LOCK_EX);
        if(bp.from == count_col(wy)-1){
            rewind(wy);

            for(int i=0;i<a_row;i++){
                if(fgetc(wy)!=EOF){
                while(fgetc(wy)!= '\0' ){printf("xddd");}; //find end of line
                }

                for(int j=0;j<wid;j++)
                    fprintf(wy," %d",result[i][j]);
                fputc('\0',wy);

            }
        }
        flock(fd,LOCK_UN);
    }
    else if(w_method==2){



    }
    else{
        perror("wrong index of write method (choode 1 or 2");
    }

    free(a_matrix);
    free(b_matrix);
    return 1;
}




void ps_work(char *lista,int proc, char *time, int w_method,int i){

    int  mltp_co=0;
    FILE *list=fopen(lista, "r");

    if( list == NULL ){
       perror("cannot open file");
       return;
    }


    char *line = (char *)calloc(255,sizeof(char));

    while(fgets(line,255,list)!=NULL){

        struct files filee= read_files(line);                  //reads files names from line of list
        FILE *aptr=fopen(filee.a, "r");
        FILE *bptr=fopen(filee.b, "r");
        FILE *wyptr=fopen(filee.wy, "w+");

        if( aptr == NULL){
           perror("cannot open file a (ps_work)");
           exit(-1);
        }

        if( bptr == NULL){
           perror("cannot open file b (ps_work)");
           exit(-1);
        }

        if( wyptr == NULL){
           perror("cannot open file wy (ps_work)");
           exit(-1);
        }


        struct b_part bp = find_part(count_col(bptr), proc, i);
        rewind(bptr);

        if(bp.works==1)
            mltp_co+=mltplc(aptr,bptr,wyptr,bp, w_method);        //multiplication of matrices


        fclose(aptr); fclose(bptr); fclose(wyptr);
        free(filee.a);free(filee.b);free(filee.wy);
    }


    fclose(list);
    exit(mltp_co);


}
