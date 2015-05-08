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

#define BUFFER   256

#include <com.h>


#define true  1
#define false 0
typedef char bool;


void initialisation(struct sockaddr_in*, int*, bool*);
void disconnection(int);
int s_Com(int, int, const char*, int);
int str_beginwith(const char*, const char*);
int upload(const char*, int, int);
int download(const char*, int, int);


#endif
