#ifndef __COMMANDS__
#define __COMMANDS__

#include "../core/client.h"

#define HELP       "help"
#define EXIT       "exit"
#define SEND       "send "
#define LIST       "list"
#define WHO        "who"
#define CLEAR      "clear"
#define LOGOUT     "logout"
#define REMOVE     "remove"
#define RENAME     "rename"
#define UPLOAD     "upload"
#define ASKLIST    "asklist"
#define WHISPER    "whisper"
#define DOWNLOAD   "download"
#define ISPRESENT  "ispresent"
#define DISCONNECT "disconnect"
#define CHECKFOLDER "checkfolder"

#define ARGDOWNLOAD 3
#define ARGWHISPER 3
#define ARGISPRESENT 3
#define ARGASKLIST 2
#define ARGRENAME 3
#define ARGREMOVE 2


typedef struct
{
	pthread_t *cthread;
	pthread_t *sthread;
} THREADS;


void* server_communication(void*);
void* client_communication(void*);


#endif
