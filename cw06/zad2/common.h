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

#define MSG_SIZE sizeof(struct msgbuf)
#define CLIENTS_NO 32


struct msgbuf {
  long mtype;         /* typ komunikatu   */
  key_t mkey;
  int msender_id;
  int mconnect_id;
};

struct msgbufget {
  long mtype;         /* typ komunikatu   */
  key_t mkey;
  int msender_id;
  int mconnect_id;
};

int get_msg(int queue, struct msgbufget *msgbufget, int size, int type, int flag){
  if(flag==IPC_NOWAIT){
    return mq_receive(queue,msgbufget, size, NULL);
  }
  else{
    int read=0;
    while((read=mq_receive(queue,msgbufget, size, NULL))<=0);
    return read;
  }
}

int make_msg(key_t key, int flag){
  char name[24];
  sprintf(name, "/%d", (int)key);

  int newflag = 0;
  struct mq_attr attry;
  attry.mq_flags = 0;
  attry.mq_maxmsg = 8;
  attry.mq_msgsize = MSG_SIZE;

  if(flag&IPC_CREAT==1){
    newflag=O_CREAT | O_RDWR;
  }

  return mqopen(name, newflag, S_IRWXU, &attry);

}

int rm_msg(int queue, int flag, struct msqid_ds *buff ){
  return msgctl(queue,flag, &buff );
}