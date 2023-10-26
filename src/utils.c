#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h> 
#include <termios.h>

#define SA struct sockaddr

void setNonBlockingInput() {
    struct termios ttystate;

    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO; 
    ttystate.c_cc[VMIN] = 1;
    ttystate.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}
