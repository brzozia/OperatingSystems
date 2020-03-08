#include "diffoperations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct array_struct *create_array(int size){

    if(size<=0)
        return NULL;

    struct array_struct *new_array;
    new_array->array = (struct block_struct*) calloc (size, sizeof(struct block_struct));
    new_array->size = size;

    return new_array;
};


struct pair_struct *def_sequence(int size, char **input){

    struct pair_struct *sequence;
    sequence = (struct pair_struct*) calloc (size, sizeof(struct pair_struct));

    for(int i=0;i<=size;i++){
        int s=0;

        while(input[i][s]!=':')                                         //copy fileA to sequencd
            s++;

        s++;
        int name_size=s;
        sequence[i].fileA=(char*)calloc(name_size+1, sizeof(char));     //+1 for '\0'
        strncpy(sequence[i].fileA, input[i], name_size);                //does not put '\0' at the end of string
        strcpy(sequence[i].fileA, "\0");                                //adds '\0'


        s=strlen(input[i])+1;                                           //copy fileB to sequence
        sequence[i].fileB=(char*)calloc(s-name_size, sizeof(char));
        for(int j=name_size+1;j<=s;j++)                                 //adds '\0', because it's at the end
            sequence[i].fileB[j-name_size-1]=input[i][j];


    }
    return sequence;

}

void compare_pairs(int size, char **input,struct array_struct *main_array){ ///czy ja ma  zwracac tu cos?
    ///trzeba sprawdzic czy rozmiar jest dobry

    struct pair_struct *sequence = def_sequence(size,input);

    char *command=(char*) calloc(60, sizeof(char));

    for(int i=0;i<size+1;i++){
        for(int j=0;j<60;j++)
            command[j]=0;

        strcpy(command, "diff ");
        strcat(command, sequence[i].fileA);
        strcat(command, " ");
        strcat(command, sequence[i].fileB);
        strcat(command, " >> diffres.tmp");

        system(command);

        char *fname = (char*) calloc(11,sizeof(char));
        fname="diffres.tmp";

        create_blocks(fname, main_array, i);
    }
}

int create_blocks(char *fname, struct array_struct *main_array, int pair_no){

    FILE *fptr = fopen(fname,"r");

     if( fptr == NULL ){
       perror("cannot open file");
       exit(-1);
    }

    char *buffer = (char*)calloc(255, sizeof(char));
    int op_c=0;

    while(fgets(buffer,255,fptr)!=NULL){                            // counts number of edition blocks
        if(buffer[0]!=60 && buffer[0]!=62 && buffer[0]!=45)         // 60=='<' 62=='>' 45=='-'
            op_c++;
    }

    main_array->array[pair_no].block = (char**) calloc (op_c, sizeof(char*));
    main_array->array[pair_no].size=op_c;


}


int operations_counter(int index);       // returns number of operations in the given block index

void remove_block(int index);             // removes from the block array, block of a given indeks

void remove_operation(int block_index, int operation_index);     // removes from the block table cell with a given index, operation with asecond given index
