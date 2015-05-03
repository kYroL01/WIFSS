#ifndef __HEADER__
#define __HEADER__



#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define BUFFER 256
#define ENDSIG "quit"


#define true  1
#define false 0
typedef int bool;



int beginWith(char*, char*);
void upload(char*, int);



#endif
