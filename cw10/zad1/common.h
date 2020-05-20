#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/epoll.h>


#define NAME_USED 1
#define WAITING_FOR_PLAYER 2
#define CONNECTED_WITH_PLAYER 3
#define MOVE 4
#define DISCONNECT 5
#define CONNECT 6


struct message{
    char name[32];
    int msg;
    int other;
};