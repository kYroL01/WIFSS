#ifndef __CLIENT__
#define __CLIENT__


#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <ctype.h>
#include <pthread.h>

#include <com.h>
#include <str.h>


#define BUFFER 256

#define true  1
#define false 0
typedef char bool;

//BOOL
//bool _sComOn_;
bool _tunnelOpened_;

//FCT
bool init(struct sockaddr_in*, int*);
void disconnect(int);

void handle_command(const char*, int, bool*);
void communication(int, bool*);

int upload(const char*, int);
int download(const char*, int);

void startunnel(int, int);
void acceptunnel(int, int);

//THREAD
void* scom(void*);
void* clientCommand(void*);

//STRUCT


#endif
