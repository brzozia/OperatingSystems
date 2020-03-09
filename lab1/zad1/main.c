#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include "diffoperations.h"


int is_pair(char *arg){
    int colon=0;
    int plik_name=0;
    int i=0;

    if(arg[i]!=':')
        plik_name=1;

    while(arg[i]!='\0'){
        if(arg[i]==':')
            colon=1;
        i++;
    }

    if(arg[i-1]==':')
        plik_name=0;

    if(plik_name==1 && colon==1)
        return 1;
    else
        return 0;

}

double interval(clock_t start, clock_t end){
    double res = (end-start)/ CLOCKS_PER_SEC;
}

void add_to_report(FILE * report, char *op, double real_str, double real_end, struct tms *tms_s, struct tms *tms_e){


        double real=interval(real_str,real_end);
        double user =interval(tms_s->tms_utime,tms_e->tms_utime);
        double proc =interval(tms_s->tms_stime,tms_e->tms_stime);

        fprintf(report,"operation: %s", op);
        fprintf(report,"real time: %f",real);
        fprintf(report, "user time: %f", user);
        fprintf(report, "system time: %f", proc);

        printf("operation %s",op);
        printf("real time: %f", real);
        printf("user time: %f", user);
        printf("system time: %f", proc);
}


int main(int argc, char **argv){

    if(argc==1){
        printf ("program started without arguments");
        return 1;
    }
    if(argc>1 && argv[1]<=0){                   ///mozna sprawdzic czy pierwszy arg jest wgl cyfra
        printf ("wrong given array size");
        return 1;
    }


    struct tms *com_rem_start=(struct tms*) calloc(1, sizeof(struct tms));
    struct tms *block_start=(struct tms*) calloc(1, sizeof(struct tms));
    struct tms *add_remove_start=(struct tms*) calloc(1, sizeof(struct tms));
    struct tms *com_rem_end=(struct tms*) calloc(1, sizeof(struct tms));
    struct tms *block_end=(struct tms*) calloc(1, sizeof(struct tms));
    struct tms *add_remove_end=(struct tms*) calloc(1, sizeof(struct tms));
    clock_t real_com_rem_start;
    clock_t real_block_start;
    clock_t real_add_remove_start;
    clock_t real_com_rem_end;
    clock_t real_block_end;
    clock_t real_add_remove_end;

    char *filename = (char *)calloc(20,sizeof(char));
    filename="raport2.txt";
    FILE *repptr = fopen(filename,"r");

     if( repptr == NULL ){
       perror("cannot open file");
       exit(-1);
    }




    int id=2;
    struct array_struct *main_arr;

    int size;
        sscanf(argv[1], "%d", &size);
        main_arr = create_array(size);        //first argument is a size of main array, so regardless if later was the command"create_array" or not, the array will be created

    char *op=(char*)calloc(30,sizeof(char));



    while(id<argc){
        strcpy(op,"");

        if(!strcmp(argv[id], "compare_pairs")){
            int pairs_size=0,fpair=id+1, p=0;

            while(id+1<argc && is_pair(argv[++id])==1)
                pairs_size++;
            if(argc-id!=1)id--;


            struct pair_struct *sequence = def_sequence(pairs_size,argv+fpair);

            real_com_rem_start = times(com_rem_start);
            compare_pairs(pairs_size,sequence);
            real_com_rem_end = times(com_rem_end);

            for(int i=0;i<pairs_size;i++){
                real_block_start=times(block_start);
                create_blocks(i, main_arr);
                real_block_end = times(block_end);

                op="create and add block";
                add_to_report(repptr, op,real_block_start,real_block_end,block_start,block_end);
            }
            op="compare_pairs";


        }
        else if(!strcmp(argv[id], "remove_block")){
            if(argc-id<2){
                printf("wrong arguments");
                return 1;
            }
            op="remove_block";
            int block_index;
            sscanf(argv[++id], "%d", &block_index);

            real_com_rem_start = times(com_rem_start);
            remove_block(main_arr,block_index);
            real_com_rem_end = times(com_rem_end);

        }
        else if(!strcmp(argv[id], "remove_operation")){
            if(argc-id<3){
                printf("wrong arguments");
                return 1;
            }
            int block_index;
            int operation_index;
            sscanf(argv[++id], "%d", &block_index);
            sscanf(argv[++id], "%d", &operation_index);

            remove_operation(main_arr,block_index,operation_index);

        }
        else
            printf("operation %s skipped",argv[id] );


        id++;

        add_to_report(repptr,op,real_com_rem_start,real_com_rem_end,com_rem_start,com_rem_end);

    }
    fclose(repptr);

    free(filename);
    free(op);
    free(add_remove_end);
    free(add_remove_start);
    free(block_end);
    free(block_start);
    free(com_rem_end);
    free(com_rem_start);



}
//./main 2 compare_pairs a.txt:b.txt b.txt:a.txt remove_block 0 compare_pairs b.txt:b.txt
