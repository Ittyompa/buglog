#ifndef UTILS_H
#define UTILS_H


typedef struct {
	int connfd;
	int client_n;
	pthread_t thid;
	int id;
} Client;

typedef struct {
	char input[320];
	int id_sender;
	int id_reciever;
	Client client;
} Message;

extern int connections[64];
extern Client clients[64];

void setNonBlockingInput();
void construct_message(Message* msg, char* input, int id_sender, Client client);

#endif
