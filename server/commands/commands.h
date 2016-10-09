#ifndef __COM__
#define __COM__


#include "../core/server.h"

/* ACK */
#define OK        "ok"
#define FAIL      "fail"
#define ENDT      "ENDT"
#define PRESENT   "present"
#define FINISHED  "finished"

/* CMD */
#define HELP       "help"
#define EXIT       "exit"
#define QUIT       "quit"
#define STOP       "stop"
#define HALT       "halt"
#define SEND       "send "
#define WHO        "who"
#define CLEAR      "clear"
#define CLOSE      "close"
#define UPLOAD     "upload "
#define ASKLIST    "asklist "
#define WHISPER    "whisper "
#define DOWNLOAD   "download "
#define ISPRESENT  "ispresent "
#define DISCONNECT "disconnect"
#define INTERROGATIONPOINT "?"

/* SIG */
#define ENDSIG  -1

/* ARG */
#define ARGDWL 2
#define ARGUPL 3
#define ARGDCL 2


void process_command(const char*, int);
void command_cursor(void);
void* command_handler(void* data);
void* on_connection(void* data);


#endif
