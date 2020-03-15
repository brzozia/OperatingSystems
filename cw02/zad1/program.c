#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


void generate(char *outfile, int records_no, int record_len){

    FILE * fileptr;
    fileptr = fopen(outfile,"w");                                           //creates a file if it doesn't exist

    if(fileptr=NULL){
        printf("Unable to open the file");
        return;
    }

    char *record;
    record = (char*) calloc (record_len,sizeof(char));

    FILE *randfile=fopen("/dev/urandom","r");

    for(int i=0;i<records_no;i++){                                          //creates records and add them to record table
        for(int j=0;j<record_len;j++){
            char c[1];
            fread(c,sizeof(char),1,randfile);
            c[0]=c[0]%58;
            if(c[0]<0) c[0]*=(-1);
            c[0]+=65;
            record[j]=c[0];
        }
    fwrite(record,sizeof(char),record_len,fileptr);

    }
    fclose(randfile);

    fflush(fileptr);


    fclose(fileptr);
    free(record);
}

void sort_sys(char *outfile, int records_no, int record_len){

}

void sort_lib(char *outfile, int records_no, int record_len){

}

void copy_sys(char *infile, char *outfile, int records_no, int record_len){

}

void copy_lib(char *infile, char *outfile, int records_no, int record_len){

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
