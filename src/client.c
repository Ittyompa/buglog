#include <arpa/inet.h>
#include <time.h>
#include <stdint.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include "utils.h"

#define SA struct sockaddr
#define BUFF_SZ 320 

char buffer_inp_client[BUFF_SZ];
short client_id;
char nickname[32];

void* from_server(void* arg) {
    // casting argument to int pointer
    int sockfd = *(int*)arg;
    char buffer[BUFF_SZ];
    packet_t msg;
    while (1) {
        // setting buffer to zero
        bzero(buffer, sizeof(buffer));

        // waiting to recieve message from server
        if (recv(sockfd, (packet_t*)&msg, sizeof(msg), 0) == 0) return NULL;

        switch (msg.type) {
            // handle normal messages
            case 0:
                char* current_time = get_current_time();
                printf("\33[2K\r"); // deleting current line in terminal
                fflush(stdout); // flushing buffer
                printf("[%s] <%s> %s\n", current_time, msg.client.nickname, msg.input); // printing out message and sender
                printf("<%s> %s", nickname, buffer_inp_client);
                fflush(stdout);
                break;
            case 1:
                // handling manuel messages from host
                printf("\33[2K\r"); // deleting current line in terminal
                fflush(stdout); // flushing buffer
                printf("<host> %s\n", msg.input); // printing out message and sender
                printf("<%s> %s", nickname, buffer_inp_client);
                fflush(stdout);
                break;
            case 2:
                // handling join/leave alerts (removed)
                printf("\33[2K\r"); // deleting current line in terminal
                fflush(stdout); // flushing buffer
                printf("%s\n", msg.input); // printing out message and sender
                printf("<%s> %s", nickname, buffer_inp_client);
                fflush(stdout);
                break;
            case 3:
                // handling Direct Messages
                break;
            case 4:
                // handling server updates
                break;
            case 5:
                break;
        }
    }

    return NULL;
}

int start_client(int sockfd) {
    packet_t msg;
    pthread_t thid;
    int m;
    // waiting to recieve info about the client itself (id)
    if(recv(sockfd, (packet_t*)&msg, sizeof(msg), 0) == 0) {
        return 1;
    }

    printf("nickname: ");
    fflush(stdout);
    fgets(nickname, sizeof(nickname), stdin);
    for (int i = 0; i < sizeof(nickname); ++i) {
        if (nickname[i] == '\n') {
            nickname[i] = '\0';
        }
    }

    strcpy(msg.client.nickname, nickname);
    if (send(sockfd, (packet_t*)&msg, sizeof(msg), 0) == 0) {
        return 0;
    }

    endpoint_t client = msg.client;
    client_id = msg.id_reciever;

    // creating thread to handle incoming messages
    pthread_create(&thid, NULL, &from_server, &sockfd);
    setNonBlockingInput();
    srand(time(0));

    for (;;) {
        m = 0;
        printf("\n<%s> ", client.nickname);
        fflush(stdout);

        while (1) {
            // getting input
            char c = getchar();
            // checking if input is newline or NULL
            if (c == '\n' || c == 0xffffffff) {
                break;
                // chekcing if input is esc og backspace
            } else if (c == 0x8 || c == 0x7F) { 
                if (m > 0) {
                    // printing sequence to autobackspace
                    printf("\b \b"); 
                    --m;
                }
            } else {
                // printing out char
                putchar(c); 
                buffer_inp_client[m++] = c;
            }
        }
        
        if (buffer_inp_client[0] == '/') {
            int count;
            char** args = split_string(buffer_inp_client, ' ', &count);
            if (strcmp(args[0], "/dm") == 0) {
                construct_message(&msg, buffer_inp_client, client_id, 3, (endpoint_t )client);
                strcpy(msg.nickname_receiver, buffer_inp_client);
                send(sockfd, (void*)&msg, sizeof(msg), 0);
            }
        }
        if else (m > 0) {
            // constructing message for sending
            construct_message(&msg, buffer_inp_client, client_id, 0, (endpoint_t )client);
            // sending message to server
            send(sockfd, (void*)&msg, sizeof(msg), 0);
        }
        bzero(buffer_inp_client, sizeof(buffer_inp_client));
    }

    return 0;
}

