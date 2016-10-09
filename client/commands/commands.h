#ifndef __COMMANDS__
#define __COMMANDS__

#include "../core/client.h"

#define HELP       "help"
#define EXIT       "exit"
#define QUIT       "quit"
#define SEND       "send "
#define LIST       "list"
#define WHO        "who"
#define CLEAR      "clear"
#define CLOSE      "close"
#define LOGOUT     "logout"
#define REMOVE     "remove "
#define RENAME     "rename "
#define UPLOAD     "upload "
#define ASKLIST    "asklist "
#define WHISPER    "whisper "
#define DOWNLOAD   "download "
#define ISPRESENT  "ispresent "
#define DISCONNECT "disconnect"
#define CHECKFOLDER "checkfolder"
#define INTERROGATIONPOINT "?"

#define ENDSIG  -1

#define ARGDWL 3
#define ARGWHI 3
#define ARGISP 3
#define ARGASK 2
#define ARGRNA 3
#define ARGRMV 2


typedef struct
{
	pthread_t *cthread;
	pthread_t *sthread;
} THREADS;


void* server_communication(void*);
void* client_communication(void*);
bool handle_command(const char*);


#endif
