#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


void generate(char *outfile, int records_no, int record_len){

    FILE * fileptr;
    fileptr = fopen(outfile,"w");                                           //creates a file if it doesn't exist

    if(fileptr==NULL){
        printf("Unable to open the file");
        return;
    }

    char *record;
    record = (char*) calloc (record_len,sizeof(char));

    FILE *randfile=fopen("/dev/urandom","r");

    for(int i=0;i<records_no;i++){                                           //creates records and add them to record table
        for(int j=0;j<record_len-1;j++){
            char c[1];
            fread(c,sizeof(char),1,randfile);
            c[0]=c[0]%58;
            if(c[0]<0) c[0]*=(-1);
            c[0]+=65;
            record[j]=c[0];

        }
        record[record_len-1]='\n';
        fwrite(record,sizeof(char),strlen(record),fileptr);

    }
    fclose(randfile);
    fflush(fileptr);

    fclose(fileptr);
    free(record);
}

char *get_record_sys(int fp,int record_len, int id){
    char *buf = (char*)calloc(record_len,sizeof(char));
    lseek(fp,record_len*(id),SEEK_SET);
    read(fp,buf,sizeof(char)*record_len);
    return buf;
}

void swap_rec_sys(int fileptr,int record_len,int i,int j){
    char *buf1=get_record_sys(fileptr,record_len,i);
    char *buf2=get_record_sys(fileptr,record_len,j);
    lseek(fileptr,record_len*(i),SEEK_SET);
    write(fileptr,buf2,sizeof(char)*record_len);
    lseek(fileptr,record_len*(j),SEEK_SET);
    write(fileptr,buf1,sizeof(char)*record_len);

    free(buf1);
    free(buf2);
}

void quicksort_sys(int fileptr, int left, int right,int record_len){

    if(left<right){
        int i=left+1,j=right;
        char *key = get_record_sys(fileptr,record_len,left);
        while(i<j){

            while(strcmp(get_record_sys(fileptr,record_len,i),key)<0 && i<j)
                i++;
            while(strcmp(get_record_sys(fileptr,record_len,j),key)>0 && i<j)
                j--;
            if(i<j)
                swap_rec_sys(fileptr,record_len,i,j);

        }
        swap_rec_sys(fileptr,record_len,left,j-1);
        quicksort_sys(fileptr,left,j-1,record_len);
        quicksort_sys(fileptr,j,right,record_len);

}
}


void sort_sys(char *file, int records_no, int record_len){

    int fp = open(file,O_RDWR);
    if(fp==-1){
        printf("Unable to open the file %s", file);
        return;
    }

    quicksort_sys(fp,0,records_no-1,record_len);


    close(fp);

}

char *get_record(FILE *fp,int record_len, int id){
    char *buf = (char*)calloc(record_len,sizeof(char));
    fseek(fp,record_len*(id),0);
    fread(buf,sizeof(char),record_len,fp);
    return buf;
}

void swap_rec(FILE *fileptr,int record_len,int i,int j){
    char *buf1=get_record(fileptr,record_len,i);
    char *buf2=get_record(fileptr,record_len,j);
    fseek(fileptr,record_len*(i),0);
    fwrite(buf2,sizeof(char),record_len,fileptr);
    fseek(fileptr,record_len*(j),0);
    fwrite(buf1,sizeof(char),record_len,fileptr);

    free(buf1);
    free(buf2);
}


void quicksort_lib(FILE *fileptr, int left, int right,int record_len){

    if(left<right){
        int i=left+1,j=right;
        char *key = get_record(fileptr,record_len,left);
        while(i<j){

            while(strcmp(get_record(fileptr,record_len,i),key)<0 && i<j)
                i++;
            while(strcmp(get_record(fileptr,record_len,j),key)>0 && i<j)
                j--;
            if(i<j)
                swap_rec(fileptr,record_len,i,j);

        }
        swap_rec(fileptr,record_len,left,j-1);
        quicksort_lib(fileptr,left,j-1,record_len);
        quicksort_lib(fileptr,j,right,record_len);

}
}


void sort_lib(char *file, int records_no, int record_len){

    FILE *fileptr;
    fileptr = fopen(file,"r+");
    if(fileptr==NULL){
        printf("Unable to open the file %s", file);
        return;
    }

    quicksort_lib(fileptr,0,records_no-1,record_len);


    fclose(fileptr);
}

void copy_sys(char *infile, char *outfile, int records_no, int record_len){


    int filein = open(infile,O_RDONLY);
    if(filein==-1){
        printf("Unable to open the file %s", infile);
        return;
    }

    int fileout = open(outfile,O_WRONLY|O_CREAT,0644);
    if(fileout==-1){
        printf("Unable to open the file %s",outfile);
        return;
    }

    char *bufor= (char*) calloc(record_len,sizeof(char));

    while(records_no-->0){
        read(filein,bufor,sizeof(char)*record_len);
        write(fileout,bufor,sizeof(char)*record_len);
    }

    close(filein);
    close(fileout);
    free(bufor);

}

void copy_lib(char *infile, char *outfile, int records_no, int record_len){

    FILE * fileinptr;
    fileinptr = fopen(infile,"r");
    if(fileinptr==NULL){
        printf("Unable to open the file %s", infile);
        return;
    }

    FILE * fileoutptr;
    fileoutptr = fopen(outfile,"w");
    if(fileoutptr==NULL){
        printf("Unable to open the file %s",outfile);
        return;
    }

    char *bufor= (char*) calloc(record_len,sizeof(char));

    while(records_no-->0){
        fread(bufor,sizeof(char),record_len,fileinptr);
        fwrite(bufor,sizeof(char),record_len,fileoutptr);
    }

    fclose(fileinptr);
    fclose(fileoutptr);
    free(bufor);

}


int main(int argc, char **argv){

    //warunki na wejsciowe dane
    if(argc<5){
        printf("Wrong given arguments");
        return 1;
    }


    if(strcmp(argv[1], "generate")==0){
        generate(argv[2], atoi(argv[3]), atoi(argv[4]));

    }
    else if(argc==6 && strcmp(argv[1],"sort")==0){

        if(strcmp(argv[5],"sys")==0)
            sort_sys(argv[2], atoi(argv[3]), atoi(argv[4]));

        else if(strcmp(argv[5],"lib")==0)
            sort_lib(argv[2], atoi(argv[3]), atoi(argv[4]));

        else{
            printf("Wrong given arguments - do you want to make sort with system functions or library ones?");
            return 1;
        }
    }
    else if(argc==7  && strcmp(argv[1],"copy")==0){

        if(strcmp(argv[6],"sys")==0)
            copy_sys(argv[2],argv[3],atoi(argv[4]),atoi(argv[5]));

        else if(strcmp(argv[6],"lib")==0)
            copy_lib(argv[2],argv[3],atoi(argv[4]),atoi(argv[5]));

        else{
            printf("Wrong given arguments - do you want to make copy with system functions or library ones?");
            return 1;
        }
    }
    else{
        printf("Wrong given arguments");
        return 1;
    }

}
