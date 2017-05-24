#ifndef __CLIENT__
#define __CLIENT__


#include "../../commons/commons.h"
#include "../../commons/str/str.h"
#include "../commands/commands.h"


typedef struct
{
	int16_t server_sock;
	int16_t server_port;
	int8_t client_id;

} core_variables_t;

core_variables_t g_core_variables;


bool start_client(void);
void stop_client(void);
void init_global_variables(void);


#endif
