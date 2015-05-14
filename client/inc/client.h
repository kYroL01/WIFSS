#ifndef __CLIENT__
#define __CLIENT__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


#define BUFFER 256

#define true  1
#define false 0
typedef char bool;

//BOOL
bool keepGoing;
bool tunnelOpened;

//FCT
void initialisation(struct sockaddr_in*, int*, bool*);
void disconnect(int);
void handle_command(const char*, int, bool*);
int upload(const char*, int);
int download(const char*, int);
void startunnel(int, int);
void acceptunnel(int);

int str_beginwith(const char*, const char*);
int str_validation(const char*, short int);
void str_lowerCase(char*);

//THREAD
void* scom(void*);

//STRUCT


#endif
