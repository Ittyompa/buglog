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
#include <time.h>

#include "utils.h"

#define SA struct sockaddr
#define BUFF_SZ 320
#define MAX_CLIENTS 64

char buffer_inp_server[BUFF_SZ];
struct sockaddr_in cli;

void handle_leave_alert(endpoint_t client) {
    pthread_mutex_lock(&cth_lock);

    packet_t msg;
    construct_message(&msg, "[-]", 0, 2, client);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        send(clients[i].connfd, (packet_t*)&msg, sizeof(msg), 0);
    }

    avail[client.client_n] = 0;
    close(clients[client.client_n].connfd);
    pthread_mutex_unlock(&cth_lock);
}

void handle_join_alert(endpoint_t client) {
    packet_t msg;
    construct_message(&msg, "[+]", 0, 2, client);

    printf("[+]");
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        send(clients[i].connfd, (packet_t*)&msg, sizeof(msg), 0);
    }
}

void* from_client(void* arg) {
    // casting arg to client type
    endpoint_t client = *(endpoint_t*)arg;
    int connfd = client.connfd;
    char buffer[BUFF_SZ];
    packet_t msg;
    FILE* fp;

    while (1) {
        
        // ensuring the buffer in empty
        bzero(buffer, sizeof(buffer));
        // waitin to recieve mesasge from the client
        int r = recv(connfd, (packet_t*)&msg, sizeof(msg), 0);
        /* checking if the client sends nothing back,
         * then it will terminate the connection
         */
        if (r == 0) {
            printf("\n[-]\n");
            fflush(stdout);
            // handle_leave_alert(client);
            close(connfd);
            return NULL;
        }

        // sending the same message to all other clients
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (i == client.client_n || avail[i] == 0) {
                continue;
            }
            if (send(clients[i].connfd, (packet_t*)&msg, sizeof(msg), 0) == 0) {
                perror("error sending");
            }
        }

        char* current_time = get_current_time();
        // clearing screen and printing
        printf("\33[2k\r");
        printf("[%s] <%s> %s\n", current_time, client.nickname, msg.input);
        printf("<host> %s", buffer_inp_server);
        fflush(stdout);

        free(current_time);
    }

    fclose(fp);
    return NULL;
}

void* start_server(void* arg) {
    endpoint_t client = *(endpoint_t*)arg;
    int connfd = client.connfd;

    // sending client info to client
    packet_t msg;
    msg.id_reciever = client.id;
    msg.type = 1;
    msg.client = client;
    if (send(connfd, (packet_t*)&msg, sizeof(msg), 0) == 0) {
        return NULL;
    }
    
    // reciving clinet back with added nickname
    if (recv(connfd, (packet_t*)&msg, sizeof(msg), 0) == 0) {
        return NULL;
    }

    strcpy(client.nickname, msg.client.nickname);
    strcpy(clients[client.client_n].nickname, msg.client.nickname);

    // creating thread to await messages from client
    pthread_t thid;
    pthread_create(&thid, NULL, from_client, &client);

    /* making sure the input field in the terminal is not blocket
     * by other processes running (from_client())
     */
    setNonBlockingInput();
    // handle_join_alert(client);

    int n;
    for (;;) {
        n = 0;
        printf("\n<host> ");
        fflush(stdout);

        // getting input
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
            // constructing and sending message
            packet_t msg;
            construct_message(&msg, buffer_inp_server, 0, 1, (endpoint_t)client); 
            int r = send(connfd, (void*)&msg, sizeof(msg), 0);
            if (r == 0) {
                return NULL;
            }
            bzero(buffer_inp_server, sizeof(buffer_inp_server));
        }
    }

    return NULL;
}

