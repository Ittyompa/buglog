#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include "utils.h"

#define SA struct sockaddr
#define BUFF_SZ 320

char buffer_inp_server[BUFF_SZ];
struct sockaddr_in cli;

void* from_client(void* arg) {
    int connfd = *(int*)arg;
    char buffer[BUFF_SZ];
    Message msg;
    while (1) {
        bzero(buffer, sizeof(buffer));
        int r = recv(connfd, (Message*)&msg, sizeof(msg), 0);
        if (r <= 0) {
            close(connfd);
            return NULL;
        }
        printf("\33[2k\r(%d): %s\n", msg.id_sender, msg.input);
        printf("You: %s", buffer_inp_server);
        fflush(stdout);
    }

    return NULL;
}

void* start_server(void* arg) {
    int connfd = *(int*)arg;
    pthread_t thid;
    pthread_create(&thid, NULL, from_client, arg);
    setNonBlockingInput();

    int n;
    for (;;) {
        n = 0;
        printf("\nYou: ");
        fflush(stdout);

        while (1) {
            char c = getchar();
            if (c == '\n' || c == EOF) {
                break;
            } else if (c == 8 || c == 127) {
                if (n > 0) {
                    printf("\b \b");
                    --n;
                }
            } else {
                putchar(c);
                buffer_inp_server[n++] = c;
            }
        }

        if (n > 0) {
            Message msg;
            construct_message(&msg, buffer_inp_server, 0); 
            send(connfd, (void*)&msg, sizeof(msg), 0);
            sleep(1);
            bzero(buffer_inp_server, sizeof(buffer_inp_server));
        }
    }

    return NULL;
}

