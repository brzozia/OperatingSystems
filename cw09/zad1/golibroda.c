#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

struct queue{
    int *chairs;
    int occ_chairs; //occupied chairs
    int eque; //index of 'last' occupied chair
    int sque; //index of  'first' occupied chair 
    int is_sleep; //if 0 - golibroda is not sleeping, if 1 - he is sleeping
};

struct queue que;
int n,k;
pthread_mutex_t queue_mut;
pthread_cond_t is_sleep_mut;



void *golibroda(){
    int cli=0;
    while(cli<n){
        pthread_mutex_lock(&queue_mut);

        while(que.is_sleep==1){
        pthread_cond_wait(&is_sleep_mut,&queue_mut);
        }

        if(que.occ_chairs>0){
            if(que.sque==k)
                que.sque=0;
            printf("Golibroda: czeka %d klientow\n",que.occ_chairs); //number of all waiting clients-including client whose hair is going to be cut; 
            printf("Golibroda: gole klienta %d\n",que.chairs[que.sque++]);
            que.occ_chairs--;
            pthread_mutex_unlock(&queue_mut);
            int s=rand()%10;
            sleep(s);
            cli++;
        }else{
            que.is_sleep++;
            pthread_mutex_unlock(&queue_mut);
            printf("Golibroda: ide spac\n");
        }       

    }
    return 0;
}

void *client(void * sid){
    int id=*(int *)sid;

    while(1){
        pthread_mutex_lock(&queue_mut);
        if(que.occ_chairs!=k || que.is_sleep==1){
            pthread_mutex_unlock(&queue_mut);
            break;
        }
        pthread_mutex_unlock(&queue_mut);
        
        printf("Zajete; %d\n",id);
        int s=rand()%5;
        sleep(s);
    }

    pthread_mutex_lock(&queue_mut);
    if(que.is_sleep==1){
        que.is_sleep--;
        pthread_cond_broadcast(&is_sleep_mut);
        printf("Budze golibrode; %d\n",id);
    }
   
    que.occ_chairs++;
    que.chairs[que.eque++]=id;
    if(que.eque==k){
        que.eque=0;
    }
    printf("Poczekalnia, wolne miejsca: %d\n",k-que.occ_chairs);
    pthread_mutex_unlock(&queue_mut);
    
    return 0;
}

int main(int argc, char ** argv){
    if(argc<3){
        printf("wrong number of aruments\n");
        return 1;
    }
    
    sscanf(argv[1],"%d",&k);
    sscanf(argv[2],"%d",&n);
    pthread_t *pth_id=(pthread_t*)calloc(n+1, sizeof(pthread_t));
    que.chairs=(int *)calloc(k,sizeof(int));
    que.occ_chairs=0;
    que.sque=0;
    que.eque=0;
    que.is_sleep=0;
    if(pthread_mutex_init(&queue_mut,NULL)!=0){
        perror("error during mutex init");
    }
    if(pthread_cond_init(&is_sleep_mut,NULL)!=0){
        perror("error during mutex sleep init");
    }

    if(pthread_create(&pth_id[0],NULL,&golibroda,NULL)!=0)
            perror("error while creating thread");

    int *pid;
    for(int i=1;i<n+1;i++){
        int s=rand()%5;
        sleep(s);
        pid=(int *)calloc(1,sizeof(int));
        *pid=i;
        if(pthread_create(&pth_id[i],NULL,&client,(void *)pid)!=0)
            perror("error while creating thread");
    }


    for(int i=0;i<n+1;i++){
        if(pthread_join(pth_id[i], NULL)!=0)
            perror("error occured during geting thread result");
    }



    pthread_cond_destroy(&is_sleep_mut);
    pthread_mutex_destroy(&queue_mut);
    free(pth_id);
    free(pid);
    free(que.chairs);
}