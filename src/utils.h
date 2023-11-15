#ifndef UTILS_H
#define UTILS_H

typedef struct {
	int connfd;
	int client_n;
	int id;
	int avail;
} Client;

typedef struct {
	char input[320];
	int id_sender;
	int id_reciever;
	Client client;
} Message;

extern Client clients[64];
extern int avail[64];
extern pthread_mutex_t pth_lock;

void setNonBlockingInput();
void construct_message(Message* msg, char* input, int id_sender, Client client);
void* check_connection(void* arg);
int request_hosts();

#endif
