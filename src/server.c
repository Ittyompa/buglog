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

char buffer_inp[BUFF_SZ];

void* handle_incoming(void* arg) {
    int connfd = *(int*)arg;
    char buffer[BUFF_SZ];
    while (1) {
	bzero(buffer, sizeof(buffer));
	int r = read(connfd, buffer, sizeof(buffer));
	if (r == 0) exit(1);
	printf("\x1b[2K"); 
	printf("Client: %s", buffer);
	fflush(stdout);
	printf("Server: ");
	fflush(stdout);
    }

    return NULL;
}

int start_server(int connfd) {
    pthread_t thid;
    pthread_create(&thid, NULL, &handle_incoming, &connfd);

    int chars;
    for (;;) {
	chars = 0;
	bzero(buffer_inp, sizeof(buffer_inp));

	printf("Server: ");
	fflush(stdout);
	while ((buffer_inp[chars++] = getchar()) != '\n') {}
	write(connfd, buffer_inp, sizeof(buffer_inp));
    }
    
    return 0;
}

int main(int argc, char** argv) {
    const int PORT = atoi(argv[1]);

    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
	printf("Failed to create socket\n");
	return 1;
    }
    printf("Created socket.\n");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
	printf("Failed to bind socket!\n");
	return 1;
    }
    printf("Binded.\n");

    if ((listen(sockfd, 5)) != 0) {
	printf("Failed to listen\n");
	return 1;
    }
    printf("Listening . . .\n");

    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
	printf("Could not accept!\n");
	return 1;
    }
    printf("Accepted!\n");

    start_server(connfd);
    close(sockfd);
    close(connfd);
    
    return 0;
}
