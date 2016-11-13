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
#define STOP       "stop"
#define SEND       "send "
#define WHO        "who"
#define CLEAR      "clear"
#define UPLOAD     "upload"
#define ASKLIST    "asklist"
#define WHISPER    "whisper"
#define DOWNLOAD   "download"
#define ISPRESENT  "ispresent"
#define DISCONNECT "disconnect"

/* ARG */
#define ARGDOWNLOAD 2
#define ARGDISCONNECT 2


void process_command(const char*, const uint8_t);
void* on_connection(void*);
void* command_handler(void*);
void* connections_handler(void*);


#endif
