#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char ** argv){
    if(argc <2){
        printf("Wrong number of arguments");
        return 1;
    }

    char path[200];
    strcpy(path, "sort ");
    strcat(path,argv[1]);

    FILE *sort_lines=popen(path, "w");
    pclose(sort_lines);

}