#ifndef UTILS_H
#define UTILS_H

#include <time.h>

enum Commands {
	DM,
};

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
	char nickname_reciever[32];
	int type;
	endpoint_t client;
} packet_t;

extern endpoint_t clients[64];
extern int avail[64];
extern pthread_mutex_t cth_lock;

void setNonBlockingInput();
void construct_message(packet_t* msg, char* input, int id_sender, int type, endpoint_t client);
char* get_current_time();
char** split_string(const char* str, char splitter, int* count);

#endif
