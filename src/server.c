#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h> 

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
	buffer_inp_server[n] = '\0';
	printf("\x1b[2K"); 
	printf("\rClient: %s", buffer);
	fflush(stdout);
	printf("\nYou: %s", buffer_inp_server);
	fflush(stdout);
    }

    return NULL;
}

int start_server(int connfd) {
    
    pthread_t thid;
    pthread_create(&thid, NULL, &from_client, &connfd);

    for (;;) {
	n = 0;
	bzero(buffer_inp_server, sizeof(buffer_inp_server));
	printf("You: ");
	fflush(stdout);
	while ((buffer_inp_server[n++] = getchar()) != '\n') {}
	fflush(stdout);
	write(connfd, buffer_inp_server, sizeof(buffer_inp_server));
    }
    
    return 0;
}

