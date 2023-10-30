#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#include "server.h"
#include "client.h"

#define SA struct sockaddr
#define MAX_CLIENTS 12

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Usage: %s -H/-C IP Port\n", argv[0]);
        return 1;
    }

    const int PORT = atoi(argv[3]);
    int sockfd;
    struct sockaddr_in servaddr;
    pthread_t thid[MAX_CLIENTS];
    int client_c = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Failed to create socket");
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[2]);
    servaddr.sin_port = htons(PORT);

    if (strcmp(argv[1], "-H") == 0) {
        if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            perror("Failed to bind socket");
            return 1;
        }
        if (listen(sockfd, 5) != 0) {
            perror("Failed to listen");
            return 1;
        }

        while (1) {
            socklen_t len;
            int connfd = accept(sockfd, (SA*)&servaddr, &len);
            if (connfd < 0) {
                perror("Could not accept");
                continue;
            }

            if (client_c < MAX_CLIENTS) {
                pthread_create(&thid[client_c], NULL, (void*)start_server, &connfd);
                ++client_c;
            } else {
                close(connfd);
                printf("Max clients reached\n");
                break; 
            }
        }

        // waiting thread
        for (int i = 0; i < client_c; i++) {
            pthread_join(thid[i], NULL);
        }
    } else if (strcmp(argv[1], "-C") == 0) {
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            perror("Connection failed");
            close(sockfd);
            return 1;
        }

        start_client(sockfd);
    }

    close(sockfd);
    return 0;
}

