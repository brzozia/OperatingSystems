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

#define MSG 6
#define INIT 5
#define CONNECT 4
#define LIST 3
#define DISCONNECT 2
#define STOP 1

#define MSG_SIZE sizeof(struct msgbufget)-8
#define CLIENTS_NO 32


struct msgbufget {
  long mtype;         /* typ komunikatu   */
  key_t mkey;
  int msender_id;
  int mconnect_id;
};

int get_msg(int queue, struct msgbufget *msg, int size, int type, int flag){
    return msgrcv(queue, msg, size, type, flag);
}


int make_msg(key_t key, int flag){
    return msgget(key,flag);
}

int rm_msg(int queue, int flag, struct msqid_ds *buff ){
  return msgctl(queue,flag, buff );
}