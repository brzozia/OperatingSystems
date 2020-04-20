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

#define MSG_SIZE sizeof(struct msgbuf)-8
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

