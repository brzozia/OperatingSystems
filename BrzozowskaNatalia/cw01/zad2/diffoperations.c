#include "diffoperations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct array_struct *create_array(int size){

    if(size<=0)
        return NULL;

    struct array_struct *new_array;
    new_array = (struct array_struct*) calloc(1, sizeof(struct array_struct));
    new_array->array = (struct block_struct*) calloc (size, sizeof(struct block_struct));
    new_array->size = size;

    return new_array;
};


struct pair_struct *def_sequence(int size, char **input){

    struct pair_struct *sequence;
    sequence = (struct pair_struct*) calloc (size, sizeof(struct pair_struct));

    for(int i=0;i<size;i++){
        int s=0;


        while(input[i][s]!=':')                                         //copy fileA to sequencd
                s++;

        int name_size=s;
        sequence[i].fileA=(char*)calloc(name_size+1, sizeof(char));     //+1 for '\0'
        strncpy(sequence[i].fileA, input[i], name_size);                //does not put '\0' at the end of string
        sequence[i].fileA[name_size]= '\0';                                //adds '\0'


        s=strlen(input[i]);                                           //copy fileB to sequence
        sequence[i].fileB=(char*)calloc(s-name_size, sizeof(char));

        for(int j=name_size+1;j<=s;j++)                                 //adds '\0', because it's at the end
            sequence[i].fileB[j-name_size-1]=input[i][j];


    }
    return sequence;

}

void compare_pairs(int size, struct pair_struct *sequence){

    char *command=(char*) calloc(60, sizeof(char));

    for(int i=0;i<size;i++){
        for(int j=0;j<60;j++)
            command[j]=0;
        char *str = (char*)calloc(size, sizeof(char));
        sprintf(str,"%d",i);

        strcpy(command, "diff ");
        strcat(command, sequence[i].fileA);
        strcat(command, " ");
        strcat(command, sequence[i].fileB);
        strcat(command, " > diffres");
        strcat(command, str);
        strcat(command, ".tmp");

        system(command);

    }
    free(command);
}

int create_blocks(int fileid, struct array_struct *main_array){

    char *tostr = (char*)calloc(fileid, sizeof(char));
        sprintf(tostr,"%d",fileid);

    char *fname = (char*) calloc(11+fileid,sizeof(char));
        strcpy(fname,"diffres");
        strcat(fname, tostr);
        strcat(fname, ".tmp");

    FILE *fptr = fopen(fname,"r");

     if( fptr == NULL ){
       perror("cannot open file");
       exit(-1);
    }

    char buffer[255];
    int op_c=0, size=1;

    while(fgets(buffer,255,fptr)!=NULL){                            // counts number of edition blocks
        if(buffer[0]!=60 && buffer[0]!=62 && buffer[0]!=45)         // 60=='<' 62=='>' 45=='-'
            op_c++;
        size+=strlen(buffer);
    }

    int pair_no=0;
    while(main_array->array[pair_no].block!=NULL)
        pair_no++;

    main_array->array[pair_no].block = (char**) calloc (op_c, sizeof(char*));
    main_array->array[pair_no].size=op_c;

    rewind(fptr);
    char *str=(char*)calloc(size,sizeof(char));
    int k=0;
    strcpy(str,"");
    char *buffer2=(char*)calloc(size,sizeof(char));


     while(fgets(buffer2,size-3,fptr)!=NULL){

        if(buffer2[0]!=60 && buffer2[0]!=6vi2 && buffer2[0]!=45){

            if(strcmp(str,"")!=0){
                main_array->array[pair_no].block[k] = (char*) calloc (strlen(str)+1, sizeof(char));
                strcpy(main_array->array[pair_no].block[k],str);
                k++;
                strcpy(str,"");
            }
            strcpy(str,buffer2);
        }
        else{
            strcat(str,buffer2);
        }
     }
     main_array->array[pair_no].block[k] = (char*) calloc (strlen(str)+1, sizeof(char));
     strcpy(main_array->array[pair_no].block[k],str);               //adds last operations

    fclose(fptr);

     free(str);
     free(tostr);
     free(fname);
     return pair_no;

}


int operations_counter(struct array_struct *main_array, int index){
    return main_array->array[index].size;
}

void remove_block(struct array_struct *main_array,int index){

    if(main_array->size<=index){
        printf("main array doesn't contain field(block) with given index - given index is bigger than array size");
        return;
    }

    if(main_array->array[index].block==NULL){
        printf("under this address is nothing, actually - element has been deleted before of never added");
        return;
    }

    free(main_array->array[index].block);

    main_array->array[index].size=0;
    main_array->array[index].block=NULL;

}

void remove_operation(struct array_struct *main_array,int block_index, int operation_index){

    if(main_array->size<=block_index){
        printf("main array doesn't contain field(block) with given index - given index is bigger than array size");
        return;
    }

    if(main_array->array[block_index].block==NULL){
        printf("under this address is nothing, actually - element(block) has been deleted before of never added");
        return;
    }

    if(main_array->array[block_index].size<=operation_index){
        printf("block array doesn't contain field(operation) with given index - given index is bigger than array size");
        return;
    }
    if(main_array->array[block_index].block[operation_index]==NULL){
        printf("under this address is nothing, actually - element(operation) has been deleted before of never added");
        return;
    }

    free(main_array->array[block_index].block[operation_index]);
    main_array->array[block_index].size--;
    main_array->array[block_index].block[operation_index]=NULL;

}


