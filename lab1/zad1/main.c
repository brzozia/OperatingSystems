#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diffoperations.h"


// run: ./main number_of_arr_elements [operations]

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



int main(int argc, char **argv){

    if(argc==1){
        printf ("program started without arguments");
        return 1;
    }
    if(argc>1 && argv[1]<=0){                   ///mozna sprawdzic czy pierwszy arg jest wgl cyfra
        printf ("wrong given array size");
        return 1;
    }

    int id=2;
    struct array_struct *main_arr;

    int size;
        sscanf(argv[1], "%d", &size);
        main_arr = create_array(size);        //first argument is a size of main array, so regardless if later was the command"create_array" or not, the array will be created

    while(id<argc){

        if(!strcmp(argv[id], "compare_pairs")){
            int pairs_size=0,fpair=id+1;

            while(id+1<argc && is_pair(argv[++id])==1)
                pairs_size++;
            if(argc-id!=1)id--;

            compare_pairs(pairs_size,argv+fpair,main_arr);

        }
        else if(!strcmp(argv[id], "remove_block")){
            if(argc-id<2){
                printf("wrong arguments");
                return 1;
            }
            int block_index;
            sscanf(argv[++id], "%d", &block_index);

            remove_block(main_arr,block_index);


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

    }

}
//./main 2 compare_pairs a.txt:b.txt b.txt:a.txt remove_block 0 compare_pairs b.txt:b.txt
