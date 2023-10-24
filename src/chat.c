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

char buffer_inp[BUFF_SZ];

void* handle_incoming(void* arg) {
	int sockfd = *(int*)arg;
	char buffer[BUFF_SZ];
	while (1) {
		bzero(buffer, sizeof(buffer));
		int r = read(sockfd, buffer, sizeof(buffer));
		if (r == 0) exit(1);
		printf("\x1b[2K"); 
		printf("\rStranger: %s", buffer);
		fflush(stdout);
		printf("You: %s", buffer_inp);
		fflush(stdout);
	}

	return NULL;
}

int start_client(int sockfd) {
	pthread_t thid;
	pthread_create(&thid, NULL, &handle_incoming, &sockfd); 

	int chars;
	for (;;) {
		chars = 0;
		bzero(buffer_inp, sizeof(buffer_inp));
		
		printf("You: ");
		fflush(stdout);
		while ((buffer_inp[chars++] = getchar()) != '\n') {}
		write(sockfd, buffer_inp, sizeof(buffer_inp));
	}

	return 0;
}

void* handle_incoming(void* arg) {
    int connfd = *(int*)arg;
    char buffer[BUFF_SZ];
    while (1) {
		bzero(buffer, sizeof(buffer));
		int r = read(connfd, buffer, sizeof(buffer));
		if (r == 0) exit(1);
		buffer_inp[n] = '\0';
		printf("\x1b[2K"); 
		printf("\rStranger: %s", buffer);
		fflush(stdout);
		printf("You: %s", buffer_inp);
		fflush(stdout);
    }

    return NULL;
}

void* handle_new_client(void* arg) {
	while (1) {
		if ((listen(sockfd, 5)) != 0) {
			printf("Failed to listen\n");
			return 1;
		}

		len = sizeof(cli);
		connfd = accept(sockfd, (SA*)&cli, &len);
		if (connfd < 0) {
			printf("Could not accept!\n");
		}
	}
	return NULL;
}

int start_server(int connfd) {
    pthread_t thid;
    pthread_create(&thid, NULL, &handle_incoming, &connfd);
	pthread_create(&thid, NULL, &handle_new_client, &connfd);

    for (;;) {
		n = 0;
		bzero(buffer_inp, sizeof(buffer_inp));
		buffer_inp[0] = 'D';

		printf("You: ");
		fflush(stdout);
		while ((buffer_inp[n++] = getchar()) != '\n') {}
		fflush(stdout);
		write(connfd, buffer_inp, sizeof(buffer_inp));
	}
    
    return 0;
}
    
