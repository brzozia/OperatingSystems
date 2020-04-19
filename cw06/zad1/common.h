#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define INIT 1
#define CONNECT 2
#define LIST 3
#define DISCONNECT 4
#define STOP 5

#define MSG_SIZE 256
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