#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <mqueue.h>

#define MSG 6
#define INIT 5
#define CONNECT 4
#define LIST 3
#define DISCONNECT 2
#define STOP 1

#define MSG_SIZE sizeof(struct msgbufget)
#define CLIENTS_NO 32


struct msgbufget {
  long mtype;         /* typ komunikatu   */
  key_t mkey;
  int msender_id;
  int mconnect_id;
  char msg[256];
};

int get_msg(int queue, struct msgbufget *msg, int size, int type, int flag){
  
  if(flag==IPC_NOWAIT){
    int read;
    read=mq_receive(queue,(char *) msg, sizeof(struct msgbufget), NULL);
      if(read==-1 && errno!=EAGAIN)
        perror("receive 1");
    
    return read;
  }
  else{
    int read=0;
    while((read=mq_receive(queue,(char *) msg,  sizeof(struct msgbufget), NULL))<=0){
      if(read==-1 && errno!=EAGAIN)
        perror("receive 2");
    }
    return read;
  }
}



int make_msg(key_t key, int flag){
  char name[24];
  sprintf(name, "/%d", (int)key);
  
  int newflag = 0;
  struct mq_attr attry;
  attry.mq_flags = O_NONBLOCK;
  attry.mq_maxmsg = 2;
  attry.mq_msgsize = sizeof(struct msgbufget);
  attry.mq_curmsgs = 1;

  newflag=O_CREAT | O_RDWR;

  int zmienna=0;
  if((zmienna=mq_open(name, newflag, S_IRUSR | S_IWUSR, &attry))==-1){
    perror("mq_open error");
  }

  if(mq_setattr(zmienna, &attry, NULL) != 0){
    perror("rerwere");
  }

  return zmienna;

}

int rm_msg(int queue, int flag, struct msqid_ds *buff ){
   return mq_close(queue);
 }