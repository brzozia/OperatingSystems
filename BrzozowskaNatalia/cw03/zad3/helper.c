#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "ps_work.c"



void make_matrix_to_file(int wid, int hei, char *fname){
    FILE *file = fopen(fname, "w");
    char line[100];
    char strnum[4];

    for(int j=0;j<hei;j++){
        strcpy(line,"");
        for(int i=0;i<wid;i++){
            int num = rand()%50;
            sprintf(strnum,"%d ",num);
            strcat(line,strnum);
        }
        line[strlen(line)-1]='\n';
        fprintf(file,"%s",line);
        
    }
    
    fclose(file);
}

void make_test(char *fname){
    FILE *fp=fopen(fname,"r");
    if( fp==NULL){
           perror("cannot open file ");
           exit(-1);
    }

    char line[1024];

    while(fgets(line,1024,fp)!=NULL){
        struct files files= read_files(line);
        FILE *a=fopen(files.a,"r");
        FILE *b=fopen(files.b,"r");
        if( a == NULL || a==NULL ){
           perror("cannot open file");
           exit(-1);
        }


        int a_col=count_col(a);
        int a_row = count_row(a);
        int b_row = count_row(b);
        int b_col = count_col(b);
        if(a_col != b_row){
            printf("wrong matrix a_width: %d, b_height: %d \n ",a_col,b_row);
            exit(-1);
        }

        
        int **a_matrix = make_matrix(a, 0, a_col);
        int **b_matrix = make_matrix(b, 0, b_col);rewind(b);

        int **result=(int**)calloc(a_row,sizeof(int*));

        for(int p=0;p<a_row;p++)
            result[p]=(int*)calloc(b_col,sizeof(int));
        
        int num=0;

        for(int p=0;p<b_col;p++){
            for(int j=0;j<a_row;j++){
                for(int i=0;i<a_col;i++){
                    num+=(a_matrix[j][i])*(b_matrix[i][p]);
                }
            result[j][p]=num;
            num=0;
            }
        }

        
        for(int p=0;p<a_row;p++)
            free(a_matrix[p]);
        free(a_matrix);

        for(int p=0;p<b_row;p++)
            free(b_matrix[p]);
        free(b_matrix);

        fclose(a);fclose(b);


        //print_matrix(result,b_col,a_row);
        //printf("end of matrix\n");



        //check results
        FILE *wy=fopen(files.wy,"r");
        if( wy==NULL){
           perror("cannot open file");
           exit(-1);
        }
        int wy_row=count_row(wy);
        int wy_col=count_col(wy);
        rewind(wy);
        if(wy_row !=a_row || wy_col != b_col){
            printf("Results are different - size of matrix doesn't match. wy: %d a:  %d, wy: %d  b: %d, %s\n",wy_row,a_row,wy_col,b_col,line);
            return;
        }

        char snum[10];int row=0;
        char numbers[1024];
        while(fgets(numbers,1024,wy)!=NULL){

            int lastpos=0, count=0,i=0;

            if(numbers[0]==' '){
                while(numbers[i]==' ')i++;
            }
            for( ;numbers[i]!='\0';i++){

                if(numbers[i]==' ' && numbers[i+1]!='\0'){
                    for(int k=0;k<=i-lastpos+1;k++)
                        snum[k]='\0';
                    strncpy(snum,numbers+lastpos,i-lastpos);
                    if(atoi(snum)!=result[row][count]){
                        printf("numbers are not the same file:%d - tab: %d in files: %s, line: %s\n",atoi(snum),result[row][count],line,numbers);
                        return;
                    }
                    
                    count++;
                    lastpos=i;
                }
            }

            row++;
        }


        for(int p=0;p<a_row;p++)
            free(result[p]);
        free(result);

       fclose(wy);
        free(files.a);free(files.b);free(files.wy);
    }
    printf("\nAll results are correct.\n\n");


    fclose(fp);
}







// nazwa ilosc_par min max plik_do zapisu co_ma_robic

int main(int argc,char **argv){
    if(argc<3){
        printf("Wrong given arguments");
        return 1;
    }

    if(strcmp(argv[1],"test")==0){
        make_test(argv[2]);
        
        return 0;
    }
    
    if(argc<5){
        printf("Wrong given arguments");
        return 1;
    }

    int couples=atoi(argv[1]),minv=atoi(argv[2]),maxv=atoi(argv[3]);
    if(minv>maxv || minv==0){
        printf("Wrong given arguments - min and max values");
        return 1;
    }
    FILE *flist = fopen(argv[4],"w");

    for(int i=0;i<couples;i++){
        int width_a_height_b = rand()%(maxv+1-minv)+minv;
        int width_b = rand()%(maxv +1 - minv) +minv;
        int height_a = rand()%(maxv +1 - minv) +minv;
        
        char name[20];
        char line[100];
        char tostr[10];
        sprintf(tostr,"%d",i);
        strcpy(name, "a");
        strcat(name, tostr);
        strcat(name,".tmp");
        make_matrix_to_file(width_a_height_b,height_a,name);
        
        strcpy(line,name);
        strcat(line, " ");
        
        strcpy(name,"b");
        strcat(name,tostr);
         strcat(name,".tmp");
        make_matrix_to_file(width_b,width_a_height_b,name);
        
        strcat(line,name);
        strcat(line, " ");
        
        strcpy(name,"res");
        strcat(name,tostr);

        strcat(line,name);
        strcat(line, ".tmp ");

        fprintf(flist,"%s\n",line);
        //printf("%s\n",line);

    }
    fclose(flist);
}