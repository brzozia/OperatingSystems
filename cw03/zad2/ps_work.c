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
    printf("char : %c\n",line[i]);
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
        exit(-1);
    }

    int c=1, i=0;
    while(line[i]!='\0'){
        if(line[i]==' ')
            c++;
        i++;
    }

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

int mltplc(FILE *a, FILE *b, char *wy, struct b_part bp, int w_method, int i){

    int a_col=count_col(a);
    int b_row = count_row(b);
    if(a_col != b_row){
        printf("wrong matrix a_width: %d, b_height: %d \n ",a_col,b_row);
        exit(-1);
    }

    int a_row = count_row(a);
    int wid=bp.to-bp.from;
    int **a_matrix = make_matrix(a, 0, a_col);

    int **b_matrix = make_matrix(b, bp.from, bp.to);rewind(b);

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

    FILE *wyptr=fopen(wy,"r+");
    rewind(wyptr);

    if(w_method==1){

        while(flock(fileno(wyptr),F_LOCK) == -1);
        

        for(int row=0;row<a_row;row++){
            
                char *line=(char*)calloc(200,sizeof(char));
                char *nline=(char*)calloc(200,sizeof(char));
                char *tostr = (char*)calloc(100, sizeof(char));
                
                int c=1,pos=0;char z;
                while(c<bp.from && !feof(wyptr)){
                    z=fgetc(wyptr);
                    pos++;
                    if(z==' ')
                        c++;
                    printf("ajaj %c %d\n",z,pos);
                }

                //int val=result[row][j];
                
                //printf("res %d\n",val);
               // if(val==0) no=1;
            //while(val>0){ val=val/10; no++;}

                fseek(wyptr, -pos, SEEK_CUR);
                fgets(line,pos+1,wyptr);
                printf("line: %s.end\n",line);
                int no=strlen(line);
                strncpy(nline,line,pos);
                printf("nline: %s\n",nline);

                for(int j=0;j<wid;j++){
                    sprintf(tostr,"%d ",result[row][j]);
                    strcat(nline,tostr);
                }
                

                fseek(wyptr,-no,SEEK_CUR);
                fgets(line,1000,wyptr);
                strncat(nline,line+pos+wid*2, strlen(line)-pos-wid*2-2);
                //strcat(nline,"\n");
                printf("nline: %s\n",nline);
                
                fseek(wyptr,-strlen(line),SEEK_CUR);
                fprintf(wyptr,"%s",nline);

                fflush(wyptr);
                //while(fgetc(wyptr)!='\n' && !feof(wyptr));
                free(nline);free(line);free(tostr);
                printf("------------------------end loop----------");
            
        }
        rewind(wyptr);
        flock(fileno(wyptr),F_ULOCK);
    }
    else if(w_method==2){

        char *filen=calloc(10,sizeof(char));
        char *tostr = (char*)calloc(i+2, sizeof(char));
        sprintf(tostr,"%d",i);

        strcpy(filen,wy);
        strcat(filen,tostr);

        FILE *fp=fopen(filen,"w");
        strcpy(filen,";");


        for(int i=0;i<a_row;i++){
            for(int j=0;j<wid;j++){
                fprintf(fp,"%d ", result[i][j]);
            }
            fseek(fp,-1,SEEK_CUR);
            fprintf(fp,"%c",'\n');
        }


        fclose(fp);fclose(wyptr);
        free(filen); free(tostr);
    }
    else{
        perror("wrong index of write method (choose 1 or 2");
    }

    free(a_matrix);
    free(b_matrix);
    return 1;
}






void ps_work(char *lista,int proc,double tim, int w_method,int i){
    time_t t_start;
    time_t t_end;
    t_start=time(NULL);

    int  mltp_co=0;
    FILE *mlt_tasks=fopen("tasks.txt","r+");
    FILE *list=fopen(lista, "r");

    if( list == NULL || mlt_tasks==NULL){
       perror("cannot open file");
       exit(-1);
    }


    char *line = (char *)calloc(255,sizeof(char));


    int my_oper=0, done=0;
    struct files filee;  struct b_part bp;
    filee.a=(char*)calloc(20,sizeof(char));
    filee.b=(char*)calloc(20,sizeof(char));
    filee.wy=(char*)calloc(20,sizeof(char));
    rewind(mlt_tasks);


    while(done>=0){

        while(fgets(line,255,mlt_tasks)!=NULL){

            int fi;char p;
            sscanf(line, "%d",&fi);

            if(line[0]=='k'){
                continue;
            }
            else if(my_oper==-1 || fi==i){
                while(flock(fileno(mlt_tasks),F_LOCK)==-1);

                int new=fi;
                int len=strlen(line);
                fseek(mlt_tasks,-len,SEEK_CUR);
                line[0]='k';
                fputs(line,mlt_tasks);
                fflush(mlt_tasks);

                flock(fileno(mlt_tasks),F_ULOCK);

                sscanf(line, "%s %s %s %d %d %d %s",&p,filee.a,filee.b,&bp.from,&bp.to,&bp.works,filee.wy );
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

                if(bp.works==1)
                    mltp_co+=mltplc(aptr,bptr,filee.wy,bp, w_method,new);
                else{
                    
                    if(w_method==2){
                        char *filen=calloc(10,sizeof(char));
                        char *tostr = (char*)calloc(new+2, sizeof(char));
                        sprintf(tostr,"%d",new);

                        strcpy(filen,filee.wy);
                        strcat(filen,tostr);
                        printf("filen: %s\n",filen);
                        FILE *fp=fopen(filen,"w");
                        fclose(fp);
                        free(filen);free(tostr);
                    }
                }
                done++;
                fclose(aptr);fclose(bptr);

                t_end=time(NULL);
                if(t_end-t_start>=tim){
                    free(line);free(filee.a);free(filee.b);
                    fclose(list);fclose(mlt_tasks);
                    printf("end of time\n");
                    exit(mltp_co);

                }
                    

            }

            if(fi==i)
                my_oper++;

            while(flock(fileno(mlt_tasks),F_LOCK)==-1);


        }
        rewind(mlt_tasks);

        if(my_oper==0)
            my_oper=-1;
        else
            my_oper=0;

        if(done==0)
            done=-1;
        else
            done=0;
    }



    free(line);free(filee.a);free(filee.b);
    fclose(list);fclose(mlt_tasks);
    exit(mltp_co);


}
