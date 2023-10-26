#include <arpa/inet.h>
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
#define BUFF_SZ 1024

char buffer_inp_client[BUFF_SZ];
int m;

void* from_server(void* arg) {
    int sockfd = *(int*)arg;
    char buffer[BUFF_SZ];
    while (1) {
        bzero(buffer, sizeof(buffer));
        int r = read(sockfd, buffer, sizeof(buffer));
        if (r == 0) exit(1);

        printf("\x1b[2K\rServer: %s\n", buffer);
        printf("You: %s", buffer_inp_client);
        fflush(stdout);
    }
    return NULL;
}

int start_client(int sockfd) {
    pthread_t thid;
    pthread_create(&thid, NULL, &from_server, &sockfd);
    setNonBlockingInput();

    for (;;) {
        m = 0;
        printf("\nYou: ");
        fflush(stdout);

        while (1) {
            char c = getchar();
            if (c == '\n' || c == EOF) {
                break;
            } else if (c == 8 || c == 127) { 
                if (m > 0) {
                    printf("\b \b"); 
                    --m;
                }
            } else {
                putchar(c); 
                buffer_inp_client[m++] = c;
            }
        }

        if (m > 0) {
            write(sockfd, buffer_inp_client, m);
            bzero(buffer_inp_client, sizeof(buffer_inp_client));
        }
    }
    return 0;
}

