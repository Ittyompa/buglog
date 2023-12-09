#ifndef UTILS_H
#define UTILS_H

typedef struct {
	int connfd;
	int client_n;
	int id;
	char nickname[32];
} endpoint_t ;

typedef struct {
	char input[320];
	int id_sender;
	int id_reciever;
	int type;
	endpoint_t client;
} Message;

extern endpoint_t clients[64];
extern int avail[64];
extern pthread_mutex_t cth_lock;

void setNonBlockingInput();
void construct_message(Message* msg, char* input, int id_sender, int type, endpoint_t client);

#endif
