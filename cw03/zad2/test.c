#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include "ps_work.c"

int main(int argc, char ** argv){

    ps_work(argv[1],atoi(argv[2]),argv[3],atoi(argv[4]),3);
}
