#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include "diffoperations.h"

#include <dlfcn.h>


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
    double res = ((double)(end-start))/ sysconf(_SC_CLK_TCK);
    return res;
}

void add_to_report(FILE * report, char *op, double real_str, double real_end, struct tms *tms_s, struct tms *tms_e){


        double real=interval(real_str,real_end);
        double user =interval(tms_s->tms_utime,tms_e->tms_utime);
        double proc =interval(tms_s->tms_stime,tms_e->tms_stime);

        fprintf(report,"operation: %s\n", op);
        fprintf(report,"real time: %f\n",real);
        fprintf(report, "user time: %f\n", user);
        fprintf(report, "system time: %f\n", proc);

        printf("operation %s\n",op);
        printf("real time: %f\n", real);
        printf("user time: %f\n", user);
        printf("system time: %f\n", proc);
}


int main(int argc, char **argv){

    if(argc==1){
        printf ("program started without arguments");
        return 1;
    }
    if(argc>1 && argv[1]<=0){                   
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

    char *filename="result3b.txt";
    FILE *repptr = fopen(filename,"a");

    void *handler = dlopen("./libdiffoperations.so",RTLD_LAZY);


    struct array_struct* (*create_array)(int size) = dlsym(handler,"create_array");
    struct pair_struct *(*def_sequence)(int size, char **input) = dlsym(handler,"def_sequence");
    void (*compare_pairs)(int size,struct pair_struct *sequence) = dlsym(handler,"compare_pairs");
    int (*create_blocks)(int fname, struct array_struct *main_array) = dlsym(handler,"create_blocks");
    int (*operations_counter)(struct array_struct *main_array,int index) = dlsym(handler,"operations_counter") ;
    void (*remove_block)(struct array_struct *main_array,int index) = dlsym(handler,"remove_block");
    void (*remove_operation)(struct array_struct *main_array,int block_index, int operation_index) = dlsym(handler,"remove_operation");

     if( repptr == NULL ){
       perror("cannot open file");
       exit(-1);
    }




    int id=2;
    struct array_struct *main_arr;

    int size;
        sscanf(argv[1], "%d", &size);
        main_arr = create_array(size);        //first argument is a size of main array, so regardless if later was the command"create_array" or not, the array will be created

    char *op;


    real_add_remove_start=times(add_remove_start);
    while(id<argc){

        if(!strcmp(argv[id], "compare_pairs")){
            int pairs_size=0,fpair=id+1;

            while(id+1<argc && is_pair(argv[++id])==1)
                pairs_size++;
            if(pairs_size>size){
                printf("to many pairs to compare! Array size is too small!");
                return 1;
            }

            if(argc-id!=1)id--;


            struct pair_struct *sequence = def_sequence(pairs_size,argv+fpair);

            real_com_rem_start = times(com_rem_start);
            compare_pairs(pairs_size,sequence);
            real_com_rem_end = times(com_rem_end);

            op="compare_pairs";
            add_to_report(repptr,op,real_com_rem_start,real_com_rem_end,com_rem_start,com_rem_end);


            for(int i=0;i<pairs_size;i++){
                real_block_start=times(block_start);
                create_blocks(i, main_arr);
                real_block_end = times(block_end);

                op="create and add block";
                add_to_report(repptr, op,real_block_start,real_block_end,block_start,block_end);
            }


        }
        else if(!strcmp(argv[id], "remove_block")){
            if(argc-id<2){
                printf("wrong arguments");
                return 1;
            }
            op="remove_block";
            int block_index;
            sscanf(argv[++id], "%d", &block_index);

            if(block_index>main_arr->size){
                printf("wrong arguments");
                return 1;
            }

            real_com_rem_start = times(com_rem_start);
            remove_block(main_arr,block_index);
            real_com_rem_end = times(com_rem_end);

            add_to_report(repptr,op,real_com_rem_start,real_com_rem_end,com_rem_start,com_rem_end);


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

            if(block_index>=main_arr->size){
                printf("wrong arguments - block index");
                return 1;
            }
            if(operation_index>=main_arr->array[block_index].size){
                printf("wrong arguments - operation index - actual no of operations is %d",operations_counter(main_arr,block_index));
                return 1;
            }

            remove_operation(main_arr,block_index,operation_index);

        }
        else
            printf("operation %s skipped",argv[id] );


        id++;


    }
    op="add and remove few times";
    real_add_remove_end=times(add_remove_end);
    add_to_report(repptr,op,real_add_remove_start,real_add_remove_end,add_remove_start,add_remove_end);


    fclose(repptr);

    free(add_remove_end);
    free(add_remove_start);
    free(block_end);
    free(block_start);
    free(com_rem_end);
    free(com_rem_start);

    dlclose(handler);


}
//./main 2 compare_pairs a.txt:b.txt b.txt:a.txt remove_block 0 compare_pairs b.txt:b.txt
