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

void* from_server(void* arg) {
    // casting argument to int pointer
    int sockfd = *(int*)arg;
    char buffer[BUFF_SZ];
    Message msg;
    while (1) {
        // setting buffer to zero
        bzero(buffer, sizeof(buffer));
        // waiting to recieve message from server
        int r = recv(sockfd, (Message*)&msg, sizeof(msg), 0);
        if (r == 0) return NULL;
        if (msg.type > 2) continue;

        printf("\33[2K\r"); // deleting current line in terminal
        fflush(stdout); // flushing buffer
        printf("[%d]> %s\n", msg.id_sender, msg.input); // printing out message and sender
        printf("[%d]> %s", client_id, buffer_inp_client);
        fflush(stdout);
    }

    return NULL;
}

int start_client(int sockfd) {
    // getting info from server
    Message msg;
    // waiting to recieve info about the client itself (id)
    int r = recv(sockfd, (Message*)&msg, sizeof(msg), 0);
    if (r == 0) exit(1);

    Client client = msg.client;
    client_id = msg.id_reciever;

    // creating thread to handle incoming messages
    pthread_t thid;
    pthread_create(&thid, NULL, &from_server, &sockfd);
    
    setNonBlockingInput();
    srand(time(0));

    int m;
    for (;;) {
        m = 0;
        printf("\n[%d]> ", client_id);
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
                // pringin out char writting
                putchar(c); 
                buffer_inp_client[m++] = c;
            }
        }

        if (m > 0) {
            Message msg;
            // constructing message for sending
            construct_message(&msg, buffer_inp_client, client_id, 0, (Client)client);
            // sending message to server
            send(sockfd, (void*)&msg, sizeof(msg), 0);
            bzero(buffer_inp_client, sizeof(buffer_inp_client));
        }
    }

    return 0;
}

