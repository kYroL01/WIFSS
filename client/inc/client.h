#ifndef __CLIENT_H
#define __CLIENT_H

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER   256

#define OK        "ok"
#define END       "quit"
#define PRESENT   "present"
#define FINISHED  "finished"
#define ISPRESENT "ispresent"

#define ENDSIG  -1
#define ASKFILE  1
#define FILEDWL  2


#define true  1
#define false 0
typedef int bool;

int str_beginwith(const char*,const char*);
int upload(const char*, int);
int download(char*, int);

#endif
