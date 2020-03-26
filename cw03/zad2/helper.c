#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


void make_matrix(int wid, int hei, char *fname){
    FILE *file = fopen(fname, "w");
    char *line=(char*)calloc(100,sizeof(char));
    char *strnum=(char*)calloc(4,sizeof(char));

    while(hei>0){
        strcpy(line,"");
        for(int i=0;i<wid;i++){
            int num = rand()%50;
            sprintf(strnum,"%d ",num);
            strcat(line,strnum);
        }
        line[strlen(line)-1]='\n';
        fprintf(file,"%s",line);
        hei--;
    }
    

    fclose(file);
    free(line);

}

// nazwa ilosc_par min max plik_do zapisu
int main(int argc,char **argv){
    if(argc<5){
        printf("Wrong given arguments");
        return 1;
    }
    
    int couples=atoi(argv[1]),minv=atoi(argv[2]),maxv=atoi(argv[3]);
    FILE *flist = fopen(argv[4],"w");

    for(int i=0;i<couples;i++){
        int width_a_height_b = rand()%(maxv+1-minv)+minv;
        int width_b = rand()%(maxv +1 - minv) +minv;
        int height_a = rand()%(maxv +1 - minv) +minv;
        
        char *name = (char *)calloc(20,sizeof(char));
        char *line=(char*)calloc(100,sizeof(char));
        char *tostr = (char*)calloc(i+1, sizeof(char));
        sprintf(tostr,"%d",i);
        strcpy(name, "a");
        strcat(name, tostr);
        make_matrix(width_a_height_b,height_a,name);
        
        strcpy(line,name);
        strcat(line, " ");
        
        strcpy(name,"b");
        strcat(name,tostr);
        make_matrix(width_b,width_a_height_b,name);
        
        strcat(line,name);
        strcat(line, " ");
        
        strcpy(name,"res");
        strcat(name,tostr);

        strcat(line,name);
        strcat(line, " ");

        fprintf(flist,"%s\n",line);
        printf("%s\n",line);


        free(line);free(tostr);free(name);
    }
    fclose(flist);

}