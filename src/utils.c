#include <arpa/inet.h> 
#include <linux/limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h> 
#include <termios.h>
#include <pthread.h>
#include <curl/curl.h>
#include <jansson.h>

#include "utils.h"

#define SA struct sockaddr
#define MAX_CLIENTS 64

// struct for arguemnt passed to a thread
typedef struct {
    int connfd;
} ThreadArgs;

Client clients[64]; // array of clients
int avail[64]; // array of available clients
pthread_mutex_t cth_lock; // variable for locking mutex when accessing global data

// non blocking function manipulating bits in tty to ensure the tty state
void setNonBlockingInput() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO; 
    ttystate.c_cc[VMIN] = 1;
    ttystate.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void construct_message(Message* msg, char* input, int id_sender, Client client) {
    strcpy(msg->input, input);
    msg->id_sender = id_sender;
    msg->client = client;
}

// checking connection of clients
void* check_connection(void* arg) {
    int err;
    socklen_t len = sizeof(err);
    
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        int ret = getsockopt(clients[i].connfd, SOL_SOCKET, SO_ERROR, &err, &len);
        if (ret == 0 && err == 0) {
            continue;
        }
    }

    return NULL;
}

// handling json data
size_t callback(void* content, size_t sz, size_t memb, void* userp) {
    size_t rsz = sz * memb;

    json_error_t err;
    json_t*  data = json_loadb(content, rsz, 0, &err);

    if (data) {
        json_dumpf(data, stdout, JSON_INDENT(2));
        printf("\n");

        json_decref(data);
    } else {
        fprintf(stderr, "Cant parse JSON: %s\n", err.text);
    }

    return rsz;
}

// sending request to server to request available hosts (not working)
int request_hosts() {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "192.168.1.71");
        // setting up function to handle response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

        json_t* json = json_object();
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, json);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            printf("Failed to perform: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}
