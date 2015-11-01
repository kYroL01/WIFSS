#ifndef __SERVER__
#define __SERVER__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <pthread.h>


#define PORT 	12443
#define BUFFER	  512
#define FGETSBUFF 256
#define TALLERCMD  34
#define PATHSIZE   32
#define MAX_CLIENTS	4
#define FREE 		1
#define TAKEN		2
#define SID		   -1

#define RUNNING		1
#define	STOPPING	2

#define true  1
#define false 0
typedef char bool;


#include <com.h>
#include <str.h>


struct client_t
{
	struct sockaddr_in addr;
	unsigned int id;
	int sock;
	int status;
};

struct client_t g_clients[MAX_CLIENTS];
unsigned int count;

pthread_t threads[MAX_CLIENTS];
pthread_t command_thread;


int isvalid(const char*);
void broadcast(int, const char*);
void close_all_connections();
int process_command(const char*, int);
void* command_handler(void* data);
void* on_connection(void* data);
int	start_server(void);


#endif
