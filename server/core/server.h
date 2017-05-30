#ifndef __SERVER__
#define __SERVER__


#include "../../commons/commons.h"
#include "../../commons/str/str.h"
#include "../commands/commands.h"
#include "../protocols/protocols.h"

#define SID -1


typedef enum
{
	FREE,
	TAKEN

} Status;

typedef struct
{
	struct sockaddr_in6 addr;
	int8_t id;
	int8_t sock;
	Status status;
	SSL *ssl;

} Client;


#define MAX_CLIENTS	8
typedef struct
{
	int8_t server_sock;
	pthread_t command_thread;
	pthread_t connections_thread;
	pthread_t threads[MAX_CLIENTS];
	Client clients[MAX_CLIENTS];
	SSL_CTX *ctx;

} CoreVariables;

CoreVariables core_variables;


void start_server(void);
void close_server(void);
void init_global_variables(void);


#endif
