#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>


#define SIGN 0
#define BLOCK 1
#define INTERLEAVED 2
#define EOFID -1

int *num;
int results[256];
int width;
int height;
int **pic;
sem_t semaf;

int *divide_sign(int pth){
    int *num=(int *)calloc(pth, sizeof(int));
    int div=256/pth;
    if(div==0){              
        for(int i=0;i<pth;i++){
            num[i]=i+1;
            if(i>=256)
                num[i]=-1;
        }
    }else{
        int rest=256-div*pth;
        num[0]=div;
        for(int i=1;i<pth;i++){
            num[i]=num[i-1]+div;
            if(i >=pth-rest)
                num[i]+=1;
        }
    }
    return num;

}

int *divide_block(int pth,FILE *fin){
    int *num=(int *)calloc(pth, sizeof(int));
    int div=ceil(width/pth);
    if(div==0)
        div=1;
    for (int j=0;j<pth;j++){
        num[j]=(j+1)*div;
        if(j>=256)
            num[j]=-1;
    }
    return num;
}

int take_num(FILE *fin){
    char *str=(char *)calloc(3,sizeof(char));
    strcpy(str,"");
    char c;
    int k=0,r=-1;
    do{
        c=fgetc(fin);
        if(c==EOF)
            return EOFID;
        if(c!='\r'&& c!='\n')
            str[k++]=c;
    }while(c!=' ' && c!='\n' );
    sscanf(str, "%d",&r);
    free(str);
    return r;
}


void *pth_sign(void * strid){
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int id,first,count=0;
    int *pid=strid;
    id=*pid;
    if(id==0)
        first=0;
    else
        first=num[id-1];

    if(num[id]!=-1){
        while(first<num[id]){
            for(int a=0;a<height;a++){
                for(int b=0;b<width;b++){
                    if(pic[a][b]==first)
                        count++;
                }
            }
        results[first]=count;
        count=0;
        first++;
        }
    }
    clock_gettime(CLOCK_REALTIME,&end);
    long int time=(end.tv_sec-start.tv_sec)*1000000 +(end.tv_nsec-start.tv_nsec)/1000;
    pthread_exit(&time);
}


void *pth_block(void *strid){
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int id,x;
    int *pid=strid;
    id=*pid;
    if(id==0)
        x=0;
    else
        x=num[id-1];

    if(num[id]!=-1){
        while(x<num[id]){
            for(int b=0;b<height;b++){
                sem_wait(&semaf);
                results[pic[b][x]]++;
                sem_post(&semaf);
            }
        x++;
        }
    }

    clock_gettime(CLOCK_REALTIME,&end);
    long int time=(end.tv_sec-start.tv_sec)*1000000 +(end.tv_nsec-start.tv_nsec)/1000;
    pthread_exit(&time);
}


void *pth_inter(void *strid){
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int id;
    int *pid=strid;
    id=*pid;

    while(id<width){
        for(int b=0;b<height;b++){
                sem_wait(&semaf);
                results[pic[b][id]]++;
                sem_post(&semaf);
            }
        id+=*num;
    }

    clock_gettime(CLOCK_REALTIME,&end);
   long long int time=(end.tv_sec-start.tv_sec)*1000000 +(end.tv_nsec-start.tv_nsec)/1000;
   //printf("%lld \n",time);
   pthread_exit(&time);
}
int maxv(){
    int maxi=0;
    for(int i=0;i<256;i++){
        if(results[i]>maxi)
            maxi=results[i];
    }
    return maxi;
}

void make_hist(FILE *fout){
    int maxi=maxv();
    fprintf(fout,"P2 \n%d %d \n255 \n",256*10,maxi);
    for(int i=maxi;i>=0;i--){
        for(int a=0;a<256;a++){
            int c=0;
            if(a<125){
                c=255;
            }
            if(results[a]<i)
                fprintf(fout, "%d %d %d %d %d %d %d %d %d %d ",c,c,c,c,c,c,c,c,c,c);
            else
            {
                fprintf(fout, "%d %d %d %d %d %d %d %d %d %d ",a,a,a,a,a,a,a,a,a,a);
            }
            
            //fprintf(fout,"color: %d, occurence %d \n",a,results[a]);
        }
        fprintf(fout,"\n");
    }
}


//----------------------------------------main---------------------

int main(int argc, char ** argv){
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    if(argc<5){
        printf("wrong number of arguments\n");
        return 1;
    }

    int pth=atoi(argv[1]);
    int way;
    FILE *fin=fopen(argv[3],"r");
    FILE *fout=fopen(argv[4],"w+");
    pthread_t *pth_id=(pthread_t*)calloc(pth, sizeof(pthread_t));
    
    fseek(fin,4,SEEK_SET);              //goes over first line
    width=take_num(fin);
    height=take_num(fin);
    take_num(fin);                      //goes over third line
    
    pic=(int**)calloc(height,sizeof(int*));

    for(int a=0;a<height;a++){
        pic[a]=(int*)calloc(width,sizeof(int));
        for(int b=0;b<width;b++){
            int p=take_num(fin);
            pic[a][b]=p;
            if(p==-1)
                b--;
        }
    }
    
    void *fun;

    if(!strcmp(argv[2],"sign")){
        way=SIGN;
        num=divide_sign(pth);
        fun=pth_sign;
        
    }
    else if(!strcmp(argv[2],"block")){
        way=BLOCK;
        num=divide_block(pth,fin);
        if(sem_init(&semaf,0,1)!=0)
            perror("error during creating semaphore\n");
        fun=pth_block;
        
    }
    else if(!strcmp(argv[2],"interleaved")){
        way=INTERLEAVED;
        if(sem_init(&semaf,0,1)!=0)
            perror("error during creating semaphore\n");
        num=(int*)calloc(1,sizeof(int));
        *num=pth;
        fun=pth_inter;
    }
    else{
        printf("wrong second argument\n");
        return 1;
    }

//-----makes threads-------
    for(int i=0;i<pth;i++){
        int *pid=(int *)calloc(1,sizeof(int));
        *pid=i;
        if(pthread_create(&pth_id[i],NULL,fun,(void *)pid)!=0)
            perror("error while creating thread");
    }

    clock_gettime(CLOCK_REALTIME,&end);

    void *ti;
    long long int *tir;

    for(int i=0;i<pth;i++){
        if(pthread_join(pth_id[i], &ti)!=0)
            perror("error occured during geting thread result");
        tir=ti;
        printf("Thread nr %d, took %lld us\n", i,*tir);
    }
    printf("Whole task took:  %ld us\n",(end.tv_sec-start.tv_sec)*1000000 +(end.tv_nsec-start.tv_nsec)/1000);
    

    // fprintf(fout,"way: %d\n",way);
    // for(int a=0;a<256;a++){
    //     fprintf(fout,"color: %d, occurence %d \n",a,results[a]);
    // }
    make_hist(fout);

    for(int a=0;a<height;a++){
        free(pic[a]);
    }

    sem_destroy(&semaf);
    fclose(fin);
    fclose(fout);
    free(pth_id);
    free(pic);
    free(num);
}