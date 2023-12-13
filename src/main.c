#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "server.h"
#include "client.h"
#include "utils.h"

#define SA struct sockaddr
#define MAX_CLIENTS 64

// counting clients connected (kinda)
int client_c = 0;
int client_n = 0;

typedef struct {
    char ip[16];
    int port;
    int host;
} Settings;

void pr_usage() {
    printf("Usage: [-C Connecting] [-H Host] [-ip <ip>] [-p <port>] [-h Help]\n");
}

void pr_help() {
    printf("Visit https://github.com/chekuga/buglog for more details.\n");
}

Settings* parse_args(int argc, char** argv) {
    Settings* fl = (Settings*)malloc(sizeof(Settings));
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-H") == 0) {
            fl->host = 1;
        }
        else if (strcmp(argv[i], "-C") == 0) {
            fl->host = 0;
        }
        else if (strcmp(argv[i], "-p") == 0 && i <= argc && argv[i + 1][0] != '-') {
            fl->port = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-ip") == 0 && i <= argc && argv[i + 1][0] != '-') {
            strcpy(fl->ip, argv[i + 1]);
        }
        else if (strcmp(argv[i], "-h") == 0) {
            pr_help();
            exit(1);
        }
    }

    return fl;
}


int main(int argc, char** argv) {
    if (argc != 6) {
        pr_usage();
        return 1;
    }

    Settings* fl = (Settings*)parse_args(argc, argv);

    srand(time(0));
    const int PORT = fl->port; 
    int sockfd;
    struct sockaddr_in servaddr; // declaring struct for handling servaddr 
    pthread_t thid[MAX_CLIENTS]; // declaring threads
    /* initing mutex var to preserve syncing with threads*/
    pthread_mutex_init(&cth_lock, NULL);

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // declaring socket
    if (sockfd == -1) {
        perror("Failed to create socket");
        return 1;
    }

    // configuring socket
    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(fl->ip);
    servaddr.sin_port = htons(PORT);

    if (fl->host == 1) {
        // binding socket
        if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            perror("Failed to bind socket");
            return 1;
        }

        // listening for a connection
        if (listen(sockfd, 5) != 0) {
            perror("Failed to listen");
            return 1;
        }

        // setting all values of avail to 0 (false)
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            avail[i] = 0;
        }

        while (1) {
            if (client_c == MAX_CLIENTS) {
                perror("max clients reached");
                exit(1);
            }

            socklen_t len;
            
            /* choosing an availafle position in the array of clients */
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (avail[i] != 1) {
                    avail[i] = 1;
                    client_n = i;
                    break;
                }
                ++client_c;
            }

            // accepting incoming connection
            pthread_mutex_lock(&cth_lock);
            clients[client_n].connfd = accept(sockfd, (SA*)&servaddr, &len);

            if (clients[client_n].connfd < 0) {
                perror("Could not accept");
                exit(1);
            }
            
            /* Configuring client settings with an availafle id 
             * as well as the client idx, and then creating
             * the thread for handling the client*/
            clients[client_n].client_n = client_n;
            clients[client_n].id = clients[client_n].client_n + 1;
            pthread_mutex_unlock(&cth_lock);

            if (client_c < MAX_CLIENTS) {
                pthread_create(&thid[client_n], NULL, (void*)start_server, &clients[client_n]);
                ++client_c;
            } else {
                printf("Max clients reached\n");
                break; 
            }
        }

        // waiting for threads to finish executing
        for (int i = 0; i < client_c; ++i) {
            pthread_join(thid[i], NULL);
        }
    }
    else if (fl->host == 0) {
        // connecting to server
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
            perror("connection failed.");
            close(sockfd);
            return 1;
        }

        start_client(sockfd);
    }
    else {
        pr_usage();
        return 1;
    }

    free(fl);
    close(sockfd);
    return 0;
}

