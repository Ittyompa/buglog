#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h> 
#include <termios.h>
#include <pthread.h>
#include "utils.h"

#define SA struct sockaddr
#define MAX_CLIENTS 64

typedef struct {
    int connfd;
} ThreadArgs;

Client clients[64];
int avail[64];
pthread_mutex_t cth_lock;

void setNonBlockingInput() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO; 
    ttystate.c_cc[VMIN] = 1;
    ttystate.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void construct_message(Message* msg, char* input, int id_sender, Client client) {
    strcpy(msg->input, input);
    msg->id_sender = id_sender;
    msg->client = client;
}

void* check_connection(void* arg) {
    int err;
    socklen_t len = sizeof(err);
    
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        int ret = getsockopt(clients[i].connfd, SOL_SOCKET, SO_ERROR, &err, &len);
        if (ret == 0 && err == 0) {
            continue;
        }
    }

    printf("Client left");

    return NULL;
}

