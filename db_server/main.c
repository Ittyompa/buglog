// ignore file test-file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

void handle_request(int sockfd) {
	char response[] = "HTTP/1.1 200 OK\r\nContent-Length: 15\r\nContent-Type: text/plain\r\n\r\nHello, world!\r\n"; // test
	send(sockfd, response, strlen(response), 0);
}

int main(void) {
	int connfd, sockfd;
	struct sockaddr_in saddr;
	int len = sizeof(saddr);

	if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Failed to create socket!");
		exit(1);
	}

	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(PORT);

	if (bind(connfd, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
		perror("failed to bind");
		exit(1);
	}

	if (listen(connfd, 3) < 0) {
		perror("failed to listen");
		exit(1);
	}

	while (1) {
		if ((sockfd = accept(connfd, (struct sockaddr*)&saddr, (socklen_t*)&len)) < 0) {
			perror("failed to accpet");
			exit(1);
		}

		handle_request(sockfd);
		close(sockfd);
	}

	return 0;
}


