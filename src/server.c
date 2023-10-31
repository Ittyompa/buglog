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
    Client client = *(Client*)arg;
    int connfd = client.connfd;
    char buffer[BUFF_SZ];
    Message msg;

    while (1) {
        bzero(buffer, sizeof(buffer));
        int r = recv(connfd, (Message*)&msg, sizeof(msg), 0);
        if (r <= 0) {
            close(connfd);
            return NULL;
        }

        for (int i = 0; i < 12; ++i) {
            send(connections[i], &msg, sizeof(msg), 0);
        }

        printf("\33[2k\r(%d): %s\n", msg.id_sender, msg.input);
        printf("You: %s", buffer_inp_server);
        fflush(stdout);
    }

    return NULL;
}

void* start_server(void* arg) {
    Client client = *(Client*)arg;
    int connfd = client.connfd;

    Message msg;
    msg.id_reciever = client.id;
    msg.client = client;
    send(connfd, (void*)&msg, sizeof(msg), 0);

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
            if (c == '\n' || c == 0xffffffff) {
                break;
            } else if (c == 0x8 || c == 0x7F) {
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
            construct_message(&msg, buffer_inp_server, 0, (Client)client); 
            send(connfd, (void*)&msg, sizeof(msg), 0);
            sleep(1);
            bzero(buffer_inp_server, sizeof(buffer_inp_server));
        }
    }

    return NULL;
}

