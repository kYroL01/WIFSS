#ifndef __COM__
#define __COM__


#include "../core/server.h"

#define WHO        "who"
#define HELP       "help"
#define EXIT       "exit"
#define SEND       "send"
#define CLEAR      "clear"
#define WHISPER    "whisper"
#define DISCONNECT "disconnect"


void process_command(const char*, const uint8_t);
void* on_connection(void*);
void* command_handler(void*);
void* connections_handler(void*);


#endif
