#ifndef __CLIENT__
#define __CLIENT__


#include "../../commons/commons.h"
#include "../../commons/str/str.h"
#include "../commands/commands.h"
#include "../files/files.h"
#include "../protocols/protocols.h"

#define PATHWORKINGDIR "/Downloads/WIFSS/"

#define LOCAL     "local"
#define LOCALHOST "localhost"
#define ADDRLOCAL "127.0.0.1"


typedef struct
{
	char server_addr[16];
	int16_t server_sock;
	int16_t server_port;
	int8_t client_id;
	char *working_dir;
} core_variables_t;

core_variables_t g_core_variables;


bool start_client(struct sockaddr_in*);
void stop_client(void);
void init_global_variables(void);


#endif
