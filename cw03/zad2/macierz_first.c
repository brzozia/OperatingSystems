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

struct ps_pack{
    int pid;
    int col_st;
    int col_no;
};


int count_columns(char *b){
    FILE *bptr=fopen(b,"r");
    if( bptr == NULL ){
       perror("cannot open file");
       exit(-1);
    }


    fclose(bptr);
}


int multi(char *a,char *b, char *wy, struct ps_pack pack){
    FILE *aptr,*bptr,*wyptr;

    aptr=fopen(a,"r");
    bptr=fopen(b,"r");
    wyptr=fopen(wy,"a");
    if( (aptr ==NULL )|| bptr==NULL ||wyptr == NULL){
       perror("cannot open file");
       exit(-1);
    }


    fclose(aptr);
    fclose(bptr);
    fclose(wyptr);
}




int main(int argc, char **argv){

    if(argc<5){
        printf("Wrong given arguments");
        return 1;
    }

    FILE *list=fopen(argv[1], "r");

    if( list == NULL ){
       perror("cannot open file");
       exit(-1);
    }


    int ps_no=atoi(argv[2]);
    struct ps_pack pids[ps_no];
    int frag;
    int ppid = getpid();
    char *line = (char *)calloc(255,sizeof(char));
    char *a=(char*)calloc(20,sizeof(char));
    char *b=(char*)calloc(20,sizeof(char));
    char *wy=(char*)calloc(20,sizeof(char));
            printf("ppid: %d\n",ppid);



    for(int i=0;i<ps_no;i++){

        if(getpid()==ppid){ ///parent
            pids[i].pid=fork();
            //printf("next id: %d\n",(int)pids[i].pid);

        }else{              ///childrens
            pids[i].pid=(int)getpid();
            //printf(" id: %d\n",(int)pids[i].pid);

            sleep(ps_no-i+1);
        }
    }



    while(fgets(line,255,list)!=NULL){

            if((int)getpid()==ppid){ ///parent

                int i=0;            //odczytuje pliki z listy
                strcpy(a,"");
                strcpy(b,"");
                strcpy(wy,"");
                while(line[i]!=' ')i++;
                strncat(a,line,i-1);
                while(line[i]!=' ')i++;
                strncat(b,line,i-1);
                while(line[i]!='\0')i++;
                strncat(wy,line,i-1);


                int p=0;
                i=0;
                int col=7;count_columns(b);       //wyznacza ile proces ma miec kolumn z b
                frag=round(col/ps_no);
                while(i<ps_no-1){
                    pids[i].col_st=p;
                    pids[i].col_no=frag;
                    p=p+frag; i++;

                }
                pids[i].col_st=p;
                pids[i].col_no=col-p;

            }

            if((int)getpid()!=ppid){ ///childrens

                int i=0;
                while((int)getpid()!=pids[i].pid) i++;

                multi(a,b,wy,pids[i]);
                printf("proces nr: %d, linia: %s",(int)getpid(),line);


            }
            //czas procesu trzeba ustawic
            //wait i zapis do raportu

    }


    fclose(list);
    free(a); free(b); free(wy); free(line);
}
