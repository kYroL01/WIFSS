#ifndef __SERVER__
#define __SERVER__


#include "../../wifss/wifss.h"
#include "../../wifss/str/str.h"
#include "../commands/commands.h"
#include "../protocols/protocols.h"

#define MAXCMD     32
#define PATHSIZE   32
#define MAX_CLIENTS	4
#define FREE 		1
#define TAKEN		2
#define SID		   -1

#define RUNNING		1
#define	STOPPING	2


typedef struct
{
	struct sockaddr_in addr;
	uint8_t id;
	int sock;
	int status;
} client_t;


/* Global variables */
uint8_t g_server_port;
pthread_t g_command_thread;
pthread_t g_threads[MAX_CLIENTS];
client_t g_clients[MAX_CLIENTS];


void start_server(void);
void close_server(int);
void broadcast(int, const char*);
void close_all_connections(void);


#endif
