#include<stdio.h>
#include<netdb.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include"server.h"
#include"client.h"

#define SA struct sockaddr

int main(int argc, char** argv) {
	if (argc != 4) {
		printf("Too few arguments!\n");
		return 1;
	}

	const int PORT = atoi(argv[3]);
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Failed to create socket!\n");
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[2]);
	servaddr.sin_port = htons(PORT);

	// binder socket
	if (strcmp(argv[1], "-H") == 0) {
		if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
			printf("Failed to bind socket!\n");
			return 1;
		}
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

		start_server(connfd);
	}
	else if (strcmp(argv[1], "-C") == 0) {
		if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
			printf("Connection failed!\n");
			return 1;
		}

		start_client(sockfd);
	}

	close(sockfd);
	close(connfd);
	return 0;
}
