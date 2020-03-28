#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h> 
#include <fcntl.h>
#include <features.h>
#include <sys/resource.h>
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

void print_matrix(int **a,int wid, int hei){
    for(int i=0;i<hei;i++){
        for(int j=0;j<wid;j++){
            printf("%d ", a[i][j]);

        }
        printf("\n");
    }

}

struct files read_files(char *line){
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
    while(line[n]!=' ')n++;
    strncpy(file.b,line+i,n-i);
    n++;
    i=n;
    while(line[i]!='\0' && line[i]!=' ')i++;
    if(line[i]=='\0')
    strncat(file.wy,line+n,i-n-1);
    else
    strncat(file.wy,line+n,i-n);
    return file;
}

struct b_part find_part(int col, int ps_no, int i){
    struct b_part bp;
    int step = round(col/ps_no);
    if(step==0 && col != 0) step=1;

    if(i*step<col && (i*step+step>=col || ps_no==i+1)){
        bp.from=i*step;
        bp.to=col;
        bp.works=1;
    }
    else if( i*step+step<col){
        bp.from=i*step;
        bp.to=i*step+step;
        bp.works=1;
    }
    else
        bp.works=0;

    return bp;
}

int count_col(FILE *bptr){
    rewind(bptr);
    char line[10000];
    fgets(line,10000,bptr);
    if(line==NULL){
        printf("File b is empty");
        exit(-1);
    }

    int c=1, i=0;
    if(line[0]==' '){
        while(line[i]==' ')i++;
    }
    while(line[i]!='\0' && line[i+1]!='\0'){
        if(line[i]==' ' && line[i+1]!='\n')
            c++;
        i++;
    }

    return c;
}

int count_row(FILE *a){
    int hei =0;
    rewind(a);
    char line[10000];

    while(fgets(line,10000,a)!=NULL) hei++;

    rewind(a);
    return hei;
}

int **make_matrix(FILE *a, int firstc, int lastc){
    int wid = lastc-firstc; // number of cells widely
    int hei =count_row(a);
    char line[10000];
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

    return matrix;
}

int mltplc(FILE *a, FILE *b, char *wy, struct b_part bp, int w_method, int i){

    int a_col=count_col(a);
    int a_row = count_row(a);
    int b_row = count_row(b);
    if(a_col != b_row){
        printf("wrong matrix a_width: %d, b_height: %d \n ",a_col,b_row);
        exit(-1);
    }

    
    int wid=bp.to-bp.from;
    int **a_matrix = make_matrix(a, 0, a_col);

    int **b_matrix = make_matrix(b, bp.from, bp.to);rewind(b);

    int **result=(int**)calloc(a_row,sizeof(int*));

    for(int p=0;p<a_row;p++)
        result[p]=(int*)calloc(wid,sizeof(int));
    
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


    FILE *wyptr=fopen(wy,"r+");
    rewind(wyptr);
    char line[8192];
    char nline[200];
    char tostr[100];

    if(w_method==1){

        while(flock(fileno(wyptr),LOCK_EX) == -1);
        
        for(int row=0;row<a_row;row++){
                rewind(wyptr);
                strcpy(line,"");strcpy(nline,"");strcpy(tostr,"");
                
                for(int pom=0;pom<row;pom++){
                    while(fgetc(wyptr)!='\n');
                }
                
                int c=0,pos=0;char z;
                while(c<bp.from && !feof(wyptr)){
                    z=fgetc(wyptr);
                    pos++;
                    if(z==' ')
                        c++;
                }

                fseek(wyptr, -pos, SEEK_CUR);
                int no=fread(line,sizeof(char),pos,wyptr);
                line[no] = '\0';
                strcpy(nline,line);

                for(int j=0;j<wid;j++){
                    sprintf(tostr,"%d ",result[row][j]);
                    strcat(nline,tostr);
                }
                
                strcpy(line,"");
                fseek(wyptr,-no,SEEK_CUR);
                no=fread(line,sizeof(char),8192,wyptr);
                strncat(nline,line+pos+wid*2, no-pos-wid*2);
                
                fseek(wyptr,-no,SEEK_CUR);
                fprintf(wyptr,"%s",nline);

                fflush(wyptr);
                
        }
        rewind(wyptr);
        flock(fileno(wyptr),LOCK_UN);
    }
    else if(w_method==2){

        strcpy(nline,"");strcpy(tostr,"");

        sprintf(tostr,"%d",i);

        strcpy(nline,wy);
        strcat(nline,tostr);

        FILE *fp=fopen(nline,"w");
        strcpy(nline,";");


        for(int i=0;i<a_row;i++){
            for(int j=0;j<wid;j++){
                fprintf(fp,"%d ", result[i][j]);
            }
            fseek(fp,-1,SEEK_CUR);
            fprintf(fp,"%c",'\n');
        }


        fclose(fp);fclose(wyptr);
        
    }
    else{
        perror("wrong index of write method (choose 1 or 2");
    }
    
    for(int p=0;p<a_row;p++)
        free(a_matrix[p]);
    free(a_matrix);

    for(int p=0;p<b_row;p++)
        free(b_matrix[p]);
    free(b_matrix);

    for(int p=0;p<a_row;p++)
        free(result[p]);
    free(result);
    
    return 1;
}






void ps_work(char *lista,int proc,double tim, int w_method,int i,double timer,double memoryr){

    time_t t_start;
    time_t t_end;
    struct rlimit time_limit,mem_limit;
    time_limit.rlim_max=timer;
    mem_limit.rlim_max=memoryr*1000000; //in bytes
    setrlimit(RLIMIT_CPU,&time_limit);
    setrlimit(RLIMIT_AS,&mem_limit);

    t_start=time(NULL);

    int  mltp_co=0;
    int mlt_desc = open("tasks.txt", O_RDWR);
    FILE *mlt_tasks=fdopen(mlt_desc,"r+");

    if( mlt_tasks==NULL){
       perror("cannot open file");
       exit(-1);
    }


    char line[300];

    int my_oper=0, done=0;
    struct files filee;  struct b_part bp;
    filee.a=(char*)calloc(20,sizeof(char));
    filee.b=(char*)calloc(20,sizeof(char));
    filee.wy=(char*)calloc(20,sizeof(char));
    rewind(mlt_tasks);

    
    

    while(done>=0){

        while(flock(fileno(mlt_tasks),LOCK_EX)== -1);

        while(fgets(line,255,mlt_tasks)!=NULL){
            

            int fi;char p;
            sscanf(line, "%d",&fi);

            if(line[0]=='k'){
                strcpy(line,"");
                continue;
            }
            else if(my_oper==-1 || fi==i){
                while(flock(fileno(mlt_tasks),LOCK_EX)==-1);

                int new=fi;
                int len=0;
                while(line[len]!='\0')len++;
                fseek(mlt_tasks,-len,SEEK_CUR);
                line[0]='k';
                fputs(line,mlt_tasks);
                fflush(mlt_tasks);

                flock(fileno(mlt_tasks),LOCK_UN);
                sscanf(line, "%s %s %s %d %d %d %s",&p,filee.a,filee.b,&bp.from,&bp.to,&bp.works,filee.wy );
                

                if(bp.works==1){

                    FILE *aptr=fopen(filee.a, "r");
                    FILE *bptr=fopen(filee.b, "r");

                    if( bptr == NULL){
                    perror("cannot open file b (ps_work)");
                    exit(-1);
                    }
                    if( aptr == NULL){
                    perror("cannot open file b (ps_work)");
                    exit(-1);
                    }

                    mltp_co+=mltplc(aptr,bptr,filee.wy,bp, w_method,new);
                    done++;
                    fclose(aptr);fclose(bptr);
                }

            }

            if(fi==i && bp.works==1)
                my_oper++;

            t_end=time(NULL);
            if(t_end-t_start>=tim){
                free(filee.a);free(filee.b);
               fclose(mlt_tasks);
                printf("end of time\n");
                exit(mltp_co);
            }

            strcpy(line,"");
             while(flock(mlt_desc,LOCK_EX)== -1);
        }

        rewind(mlt_tasks);

            my_oper=-1;
         

        if(done==0)
            done=-1;
        else
            done=0;

        t_end=time(NULL);
            if(t_end-t_start>=tim){
                free(filee.a);free(filee.b);
               fclose(mlt_tasks);
                printf("end of time\n");
                exit(mltp_co);
            }
    }
    t_end=time(NULL);
    //printf("Process %d used %ld seconds\n",getpid(),t_end-t_start);
    free(filee.a);free(filee.b);
    fclose(mlt_tasks);
    exit(mltp_co);


}
