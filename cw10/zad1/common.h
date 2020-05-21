#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/epoll.h>

#define O 0
#define X 1
#define WAITING_FOR_PLAYER 2
#define CONNECTED_WITH_PLAYER 3
#define MOVE 4
#define DISCONNECT 5
#define CONNECT 6
#define CANNOT_MOVE_THERE 7
#define IN_USE 8
#define FREE 9
#define ERROR -1

// struct game{
//     int matrix[3][3];
//     int in_use;
// };

struct message{
    char name[32];
    int msg;
    int type;
};

