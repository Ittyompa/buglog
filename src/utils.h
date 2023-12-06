#ifndef UTILS_H
#define UTILS_H

typedef struct {
	int connfd;
	int client_n;
	int id;
} Client;

typedef struct {
	char input[320];
	int id_sender;
	int id_reciever;
	int type;
	Client client;
} Message;

extern Client clients[64];
extern int avail[64];
extern pthread_mutex_t cth_lock;

void setNonBlockingInput();
void construct_message(Message* msg, char* input, int id_sender, int type, Client client);

#endif
