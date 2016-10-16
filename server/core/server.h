#ifndef __SERVER__
#define __SERVER__


#include "../../wifss/wifss.h"
#include "../../wifss/str/str.h"
#include "../commands/commands.h"
#include "../protocols/protocols.h"

#define SID -1


typedef enum
{
	FREE,
	TAKEN
} STATUS;


typedef struct
{
	struct sockaddr_in addr;
	int8_t id;
	int8_t sock;
	STATUS status;
} client_t;


#define MAX_CLIENTS	8
typedef struct
{
	int8_t server_port;
	int8_t server_sock;
	pthread_t command_thread;
	pthread_t connections_thread;
	pthread_t threads[MAX_CLIENTS];
	client_t clients[MAX_CLIENTS];	
} core_variables_t;

core_variables_t g_core_variables;


void start_server(void);
void close_server(void);
void init_global_variables(void);


#endif
