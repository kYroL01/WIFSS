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


#define BUFFER	  512
#define FGETSBUFF 256
#define MAXCMD     32
#define PATHSIZE   32
#define MAX_CLIENTS	4
#define FREE 		1
#define TAKEN		2
#define SID		   -1

#define RUNNING		1
#define	STOPPING	2

#define true  1
#define false 0


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

pthread_t threads[MAX_CLIENTS];
pthread_t command_thread;


//START / STOP
void startServer(void);
void closeServer(int);

//FUNCTION MANAGEMENT
void broadcast(int, const char*);
void close_all_connections();

//COMMANDS
void process_command(const char*, int);
void commandCursor(void);
void download(const char*, int);
void message(const char*, int);
void whisper(const char*, int);
void disconnect(const char*);

//THREADS
void* command_handler(void* data);
void* on_connection(void* data);


#endif
