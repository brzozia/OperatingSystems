#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

#define O 0
#define X 1
#define WAITING_FOR_PLAYER 2
#define PING 3
#define MOVE 4
#define DISCONNECT 5
#define CONNECT 6
#define GIVE_NAME 8
#define FREE 9
#define WINNER 10
#define NOT_WINNER 11
#define ERROR -1
#define CTRLC 12
#define REMIS 13


#define MAX_CLIENTS 14
#define UNIX_PATH_MAX  108


struct message{
    char name[32];
    int msg;
    int type;
    int other;
};

int msg_size = sizeof(struct message);