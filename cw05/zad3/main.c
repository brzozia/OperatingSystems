#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>



// przekazywane arg - 'sciezka do fifo' dla konsumenta 'plik' 'n' dla producentow 'plik' 'n' x5
int main(int argc, char **argv){

    if(argc<13){
        printf("Wrong number of arguments\n");
        return 1;
    }
    
    mkfifo(argv[1],660);

    if(fork()==0){
        execl("./consument", "./consument",argv[1], argv[2], argv[3],NULL);
        exit(0);
    }
    

    for(int i=0;i<5;i++){
        if(fork()==0){
            execl("./producent", "./producent",argv[1], argv[i*2+4], argv[i*2+5],NULL);
            exit(0);
            
        }
        
    }
    
   while((wait(NULL))!=-1);
}