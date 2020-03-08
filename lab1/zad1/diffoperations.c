#include "diffoperations.h"
#include <stdlib.h>
#include <string.h>


struct array_struct *create_array(int size)
{

    if(size<=0)
        return NULL;

    struct array_struct *new_array;
    new_array->array = (char**) calloc (size, sizeof(struct block_struct));
    new_array->size = size;

    return new_array;
};


struct pair_struct *def_sequence(int size, char **input){

    struct pair_struct *sequence;
    sequence = (struct pair_struct*) calloc (size, sizeof(struct pair_struct));

    for(int i=0;i<=size;i++){               //loop which passes over input array considering each pair
        int s=0;

        while(input[i][s]!=':'){            //copy fileA to sequencd
            s++;
        }

        s++;
        int name_size=s;
        sequence[i].fileA=(char*)calloc(name_size+1, sizeof(char)); //+1 for '\0'
        strncpy(sequence[i].fileA, input[i], name_size);  //does not put '\0' at the end of string
        strcpy(sequence[i].fileA, "\0");                   //adds '\0'


        s=strlen(input[i])+1;                           //copy fileB to sequence
        sequence[i].fileB=(char*)calloc(s-name_size, sizeof(char));
        for(int j=name_size+1;j<=s;j++){                    //adds '\0', because it's at the end
            sequence[i].fileB[j-name_size-1]=input[i][j];
        }

    }
    return sequence;

}

void compare_pairs(int size, char **input){ ///czy ja ma  zwracac tu cos?
    ///trzeba sprawdzic czy rozmiar jest dobry

    struct pair_struct *sequence = def_sequence(size,input);

    char *command=(char*) calloc(60, sizeof(char));

    for(int i=0;i<size+1;i++){
        for(int j=0;j<60;j++){
            command[j]=0;
        }
        strcpy(command, "diff ");
        strcat(command, sequence[i].fileA);
        strcat(command, " ");
        strcat(command, sequence[i].fileB);
        strcat(command, " >> diffres.tmp");

        system(command);

        char *fname = (char*) calloc(11,sizeof(char));
        *fname="diffres.tmp";
    }
}

int create_blocks(FILE *fptr){

}


int operations_counter(int index);       // returns number of operations in the given block index

void remove_block(int index);             // removes from the block array, block of a given indeks

void remove_operation(int block_index, int operation_index);     // removes from the block table cell with a given index, operation with asecond given index
