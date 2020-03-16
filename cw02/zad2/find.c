#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>


//find nazwa_katalogu [-atime arg -mtime arg -maxdepth arg]

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

void find(char *dest, int mt, int at, int maxd){

  if(maxd>=0){
    DIR *dir = opendir(dest);
    if(dir==NULL){
        printf("Cannot open the direction");
        return;
    }
    char *curr_dir=(char*)calloc(256, sizeof(char));
    getcwd(curr_dir,256);

    struct stat *el_stat=(struct stat*)calloc (1,sizeof(struct stat));
    struct dirent *el;
    time_t current_date;
    time(&current_date);




    while((el=readdir(dir))!=NULL){

        strcat(curr_dir,"/");
        strcat(curr_dir,el->d_name);
        stat(curr_dir,el_stat);
        time_t atime=(el_stat->st_atime);
        time_t mtime=(el_stat->st_mtime);


        if(at!=0 && mt!=0 && compare(difftime(atime,current_date)/(60*60*24),at)==1 && compare(difftime(mtime,current_date)/(60*60*24),at)==1){
            printf("path: %s, link: %d",curr_dir,(int)el_stat->st_nlink);
        }
        else if(mt!=0 && compare(difftime(mtime,current_date)/(60*60*24),at)==1){

        }
        else if(at!=0 && compare(difftime(atime,current_date)/(60*60*24),at)==1){

        }

    }



    free(el_stat);
    closedir(dir);

    }
}





int main(int argc,char **argv){

    if(argc<2 || argc%2!=0){
        printf("Wrong given arguments");
        return 1;
    }

    char *dest = argv[1];
    int mt=0,at=0,maxd=0;


    for(int i=2;i<argc;i++){

        if(argc-i>1 && strcmp(argv[i],"-mtime")==0){
            mt=atoi(argv[++i]);                                     //can manage with arguments '+1' '-1'
        }
        else if(argc-i>1 && strcmp(argv[i],"-atime")==0){
            at=atoi(argv[++i]);
        }
        else if(argc-i>1 && strcmp(argv[i],"-maxdepth")==0){
            maxd=atoi(argv[++i]);
        }
        else{
            printf("Wrong given arguments");
            return 1;
        }
    }

    find(dest,mt,at,maxd);



}
