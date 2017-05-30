#ifndef __COMMANDS__
#define __COMMANDS__

#include "../core/client.h"

#define WHO         "who"
#define HELP        "help"
#define EXIT        "exit"
#define SEND        "send"
#define CLEAR       "clear"
#define REMOVE      "remove"
#define RENAME      "rename"
#define WHISPER     "whisper"
#define DISCONNECT  "disconnect"


typedef struct
{
	pthread_t *cthread;
	pthread_t *sthread;

} Threads;


void* server_communication(void*);
void* client_communication(void*);


#endif
