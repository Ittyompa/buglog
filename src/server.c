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
#include "utils.h"

#define SA struct sockaddr
#define BUFF_SZ 1024

char buffer_inp_server[BUFF_SZ];
int n;

void* from_client(void* arg) {
    int connfd = *(int*)arg;
    char buffer[BUFF_SZ];
    while (1) {
	bzero(buffer, sizeof(buffer));
	int r = read(connfd, buffer, sizeof(buffer));
	if (r == 0) exit(1);
	printf("\x1b[2K\rClient: %s\n", buffer);
	printf("You: %s", buffer_inp_server);
	fflush(stdout);
    }

    return NULL;
}

int start_server(int connfd) {
    
    pthread_t thid;
    pthread_create(&thid, NULL, &from_client, &connfd);
    setNonBlockingInput();

    for (;;) {
	n = 0;
	printf("\nYou: ");
	fflush(stdout);

	while (1) {
	    char c = getchar();
	    if (c == '\n' || c == EOF) {
		break;
	    }
	    else if (c == 8 || c == 127) {
		if (n > 0) {
		    printf("\b \b");
		    --n;
		}
	    } 
	    else {
		putchar(c);
		buffer_inp_server[n++] = c;
	    }
	}

	if (n > 0) {
	    write(connfd, buffer_inp_server, n);
	    bzero(buffer_inp_server, sizeof(buffer_inp_server));
	}
    }
    
    return 0;
}

