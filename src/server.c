#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"

#define SA struct sockaddr
#define BUFF_SZ 320
#define MAX_CLIENTS 64

char buffer_inp_server[BUFF_SZ];
struct sockaddr_in cli;

void* from_client(void* arg) {
    // casting arg to client type
    Client client = *(Client*)arg;
    int connfd = client.connfd;
    char buffer[BUFF_SZ];
    Message msg;
    FILE* fp;

    while (1) {
        
        // ensuring the buffer in empty
        bzero(buffer, sizeof(buffer));
        // waitin to recieve mesasge from the client
        int r = recv(connfd, (Message*)&msg, sizeof(msg), 0);
        if (r == 0) {
            printf("%d left the chat.\n", client.id);
            fflush(stdout);
            avail[client.client_n] = 0;
            close(connfd);
            return NULL;
        }

        // sending the  same message to all other clients
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (i == client.client_n) {
                continue;
            }
            int r = send(clients[i].connfd, (Message*)&msg, sizeof(msg), 0);
            if (r == 0) {
                printf("Error echoing\n");
            }
        }

        // clearing screen and printing
        printf("\33[2k\r(%d): %s\n", msg.id_sender, msg.input);

        printf("[0]> %s", buffer_inp_server);
        fflush(stdout);
    }

    fclose(fp);
    return NULL;
}

void* start_server(void* arg) {
    Client client = *(Client*)arg;
    int connfd = client.connfd;

    // sending clinet info to client
    Message msg;
    msg.id_reciever = client.id;
    msg.type = 1;
    msg.client = client;
    send(connfd, (Message*)&msg, sizeof(msg), 0);

    //  creating thread to await messages from client
    pthread_t thid;
    pthread_create(&thid, NULL, from_client, arg);

    // making sure the input field in the terminal is not blocket
    setNonBlockingInput();

    char* join_msg; 
    // printing join messsage
    asprintf(&join_msg, "\e[1m%d joined the chat\e[m", client.id);
    printf("%s", join_msg);
    strcpy(msg.input, join_msg);
    free(join_msg);
    msg.type = 1;
    // sending join message to all clients
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        send(clients[i].connfd, (Message*)&msg, sizeof(msg), 0);
    }

    int n;
    for (;;) {
        n = 0;
        printf("\n[0]> ");
        fflush(stdout);

        // getting input
        while (1) {

            char c = getchar();
            // looking for enter or backspace keys
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
            // construcing and sending message
            Message msg;
            construct_message(&msg, buffer_inp_server, 0, 0, (Client)client); 
            // sending message as type void
            int r = send(connfd, (void*)&msg, sizeof(msg), 0);
            if (r == 0) {
                return NULL;
            }
            bzero(buffer_inp_server, sizeof(buffer_inp_server));
        }
    }

    return NULL;
}

