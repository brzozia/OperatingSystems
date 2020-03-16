#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ftw.h>
#include <time.h>
#include <sys/stat.h>

    char *dest;
    int mt=0;
    int at=0;
    int maxd=500;
    int sa=0;
    int sm=0;


int compare(double diff,int t){

    if(t<0){
        t*=(-1);
        if(diff<t)
            return 1;
        else
            return 0;
    }
    else if(t>0){
        if(diff>t)
            return 1;
        else
            return 0;
    }
    else{
        if(diff==t)
            return 1;
        else
            return 0;
    }

}

int func(const char *name,const struct stat * el_stat, int type, struct FTW *tfw){

    if(tfw->level > maxd)
    return 0;

    time_t current_date;
    time(&current_date);

    if(sa==1 && compare(difftime(current_date,el_stat->st_atime)/(60*60*24),at)!=1)
        return 0;
    if(sm==1 && compare(difftime(current_date,el_stat->st_mtime)/(60*60*24),mt)!=1)
        return 0;


    char * curr_dir=(char*)calloc(256,sizeof(char));

        strcpy(curr_dir,dest);
        strcat(curr_dir,"/");
        strcat(curr_dir,name);

     printf("path: %s, link: %d ",curr_dir,(int)el_stat->st_nlink);

            if(S_ISREG(el_stat->st_mode))
                printf("type: file ");
            else if(S_ISDIR(el_stat->st_mode))
                printf("type: dir ");
            else if(S_ISCHR(el_stat->st_mode))
                printf("type: char dev ");
            else if(S_ISBLK(el_stat->st_mode))
                printf("type: block dev ");
            else if(S_ISFIFO(el_stat->st_mode))
                printf("type: fifo ");
            else if(S_ISLNK(el_stat->st_mode))
                printf("type: slink ");
            else if(S_ISSOCK(el_stat->st_mode))
                printf("type: socket ");

    printf("size: %d B last access: %s last modif: %s\n", (int)el_stat->st_size, ctime(&el_stat->st_atime),ctime(&el_stat->st_mtime));
    return 0;

}

void find2(){

    nftw(dest,func,50,FTW_PHYS);

}

int main(int argc, char ** argv){

    if(argc<2 || argc%2!=0){
        printf("Wrong given arguments");
        return 1;
    }

    dest = argv[1];


    for(int i=2;i<argc;i++){

        if(argc-i>1 && strcmp(argv[i],"-mtime")==0){
            mt=atoi(argv[++i]);
            sm=1;                                 //can manage with arguments '+1' '-1'
        }
        else if(argc-i>1 && strcmp(argv[i],"-atime")==0){
            at=atoi(argv[++i]);
            sa=1;
        }
        else if(argc-i>1 && strcmp(argv[i],"-maxdepth")==0 && argv[i+1]>=0){
            maxd=atoi(argv[++i]);
        }
        else{
            printf("Wrong given arguments %s", argv[i]);
            return 1;
        }
    }

    find2();

}
