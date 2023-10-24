#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/socket.h>
#include <unistd.h> 
#include <pthread.h>
#define SA struct sockaddr
#define BUFF_SZ 1024

char buffer_inp_client[BUFF_SZ];
char tmp_buffer[BUFF_SZ];
int m;

void* from_server(void* arg) {
	int sockfd = *(int*)arg;
	char buffer[BUFF_SZ];
	while (1) {
		bzero(buffer, sizeof(buffer));
		int r = read(sockfd, buffer, sizeof(buffer));
		if (r == 0) exit(1);
		strcpy(tmp_buffer, buffer_inp_client);
		printf("\x1b[2K"); 
		fflush(stdout);
		printf("\rServer: %s", buffer);
		fflush(stdout);
		printf("You: %s", tmp_buffer);
		fflush(stdout);
	}

	return NULL;
}

int start_client(int sockfd) {
	pthread_t thid;
	pthread_create(&thid, NULL, &from_server, &sockfd); 

	for (;;) {
	    m = 0;
		printf("You: ");
		fflush(stdout);
		while ((buffer_inp_client[m++] = getchar()) != '\n') {}
		write(sockfd, buffer_inp_client, sizeof(buffer_inp_client));
	}

	return 0;
}

