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
		printf("\rServer: %s", buffer);
		fflush(stdout);
		printf("Client: ");
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
		
		printf("Client: ");
		fflush(stdout);
		while ((buffer_inp[chars++] = getchar()) != '\n') {}
		write(sockfd, buffer_inp, sizeof(buffer_inp));
	}

	return 0;
}

int main(int argc, char** argv) {
	const int PORT = atoi(argv[1]);

	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Failed to create socket!\n");
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection failed!\n");
		return 1;
	}

	int r = start_client(sockfd);	
	if (r > 0) printf("Could not start client!\n");
	close(sockfd);

	return 0;
}
