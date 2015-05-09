#ifndef __CLIENT_H
#define __CLIENT_H


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


#define BUFFER   256

#include <com.h>


#define true  1
#define false 0
typedef char bool;

bool keepGoing;

//FCT
void initialisation(struct sockaddr_in*, int*, bool*);
void disconnect(int);
int str_beginwith(const char*, const char*);
int upload(const char*, int);
int download(const char*, int);

//THREAD
void* scom(void*);



#endif

