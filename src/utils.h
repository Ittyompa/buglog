#ifndef UTILS_H
#define UTILS_H

typedef struct {
	char input[320];
	int id_sender;
} Message;

void setNonBlockingInput();
void construct_message(Message* msg, char* input, int id_sender);

#endif
