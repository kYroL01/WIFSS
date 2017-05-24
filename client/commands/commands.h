#ifndef __COMMANDS__
#define __COMMANDS__

#include "../core/client.h"

#define WHO         "who"
#define HELP        "help"
#define EXIT        "exit"
#define SEND        "send"
#define CLEAR       "clear"
#define LOGOUT      "logout"
#define REMOVE      "remove"
#define RENAME      "rename"
#define WHISPER     "whisper"
#define DISCONNECT  "disconnect"

#define ARGWHISPER   3


typedef struct
{
	pthread_t *cthread;
	pthread_t *sthread;

} THREADS;


void* server_communication(void*);
void* client_communication(void*);


#endif
