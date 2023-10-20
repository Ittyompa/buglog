#include<stdio.h>
#include<netdb.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include"chat.h"

#define BUFF_SZ 1024
#define PORT 2232

#define struct sockaddr

int main(int argc, char** argv) {

	struct sockaddr_in servaddr;

	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd == -1) {
		printf("Error creating socket");
		exit(0);
	}
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	
	if (argc == 1) {
		printf("Starting client * * *\n");
		fflush(stdout);
		start_client(socketfd, (SA*)servaddr);
	}
	else // start_server();
	
	return 0;
}
