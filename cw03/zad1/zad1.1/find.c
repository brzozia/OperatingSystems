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
        if(diff<=1)
            return 1;
        else
            return 0;
    }

}
void nice_print(char *curr_dir, struct stat *el_stat){

    printf("path: %s, link: %d ",curr_dir,(int)el_stat->st_nlink);

            if(S_ISREG(el_stat->st_mode))
                printf("type: file ");
            else if(S_ISLNK(el_stat->st_mode))
                printf("type: slink ");
            else if(S_ISDIR(el_stat->st_mode))
                printf("type: dir ");
            else if(S_ISCHR(el_stat->st_mode))
                printf("type: char dev ");
            else if(S_ISBLK(el_stat->st_mode))
                printf("type: block dev ");
            else if(S_ISFIFO(el_stat->st_mode))
                printf("type: fifo ");
            else if(S_ISSOCK(el_stat->st_mode))
                printf("type: socket ");

            printf("size: %d B last access: %s last modif: %s\n", (int)el_stat->st_size, ctime(&el_stat->st_atime),ctime(&el_stat->st_mtime));

}


void find(DIR *dir,int start,char *dest, int mt, int at, int maxd, int sa, int sm){

  if(maxd>=0 || maxd<=-5){

    char *curr_dir=(char*)calloc(256, sizeof(char));

    struct stat *el_stat=(struct stat*)calloc (1,sizeof(struct stat));
    struct dirent *el;
    time_t current_date;
    time(&current_date);



    while((el=readdir(dir))!=NULL){
        strcpy(curr_dir,dest);
        strcat(curr_dir,"/");
        strcat(curr_dir,el->d_name);
        lstat(curr_dir,el_stat);

        if(sa==1 && compare(difftime(current_date,el_stat->st_atime)/(60*60*24),at)!=1)
            continue;
        if(sm==1 && compare(difftime(current_date,el_stat->st_mtime)/(60*60*24),mt)!=1)
            continue;


        //nice_print(curr_dir,el_stat);
        if(S_ISLNK(el_stat->st_mode))
        continue;

        if(!S_ISLNK(el_stat->st_mode) && S_ISDIR(el_stat->st_mode) &&  strcmp(el->d_name,"..")!=0 && strcmp(el->d_name,".")!=0 && (maxd>0 || maxd<=-5)){
                chdir(curr_dir);
                getcwd(curr_dir,256);
                DIR *ndir = opendir(curr_dir);
                char *rel_dir=(char*)calloc(255, sizeof(char));
                strncpy(rel_dir,curr_dir+start,255);


                 int child_pid = vfork();
                 if(child_pid==0){
                        printf("\n\n%s PROCESSES path: %s ,PID: %d \n",el->d_name,rel_dir, (int)getpid());
                        execlp("ls","ls","-l",NULL);
                }
                wait(NULL);

                find(ndir,start,curr_dir,mt,at,maxd-1,sa,sm);
                closedir(ndir);
        }
    }
        free(el_stat);


    }

}





int main(int argc,char **argv){

    if(argc<2 || argc%2!=0){
        printf("Wrong given arguments");
        return 1;
    }

    char *dest = argv[1];
    int mt=0,at=0,maxd=-5;
    int sw_m=0, sw_a=0;


    for(int i=2;i<argc;i++){

        if(argc-i>1 && strcmp(argv[i],"-mtime")==0){
            mt=atoi(argv[++i]);
            sw_m=1;                                             //can manage with arguments '+1' '-1'
        }
        else if(argc-i>1 && strcmp(argv[i],"-atime")==0){
            at=atoi(argv[++i]);
            sw_a=1;
        }
        else if(argc-i>1 && strcmp(argv[i],"-maxdepth")==0 && argv[i+1]>=0){
            maxd=atoi(argv[++i]);
        }
        else{
            printf("Wrong given arguments %s", argv[i]);
            return 1;
        }
    }

    chdir(dest);
    DIR *dir = opendir(dest);

    if(dir==NULL){
        printf("Cannot open the direction");
        return 1;
    }

    getcwd(dest,256);

    find(dir,strlen(dest),dest,mt,at,maxd,sw_a,sw_m);

    closedir(dir);


}
