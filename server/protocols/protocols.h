#ifndef __PROTOCOLS__
#define __PROTOCOLS__


#include "../core/server.h"


void download(const char*, int);
void message(const char*, int);
void whisper(const char*, int);
void ask_list(const char*, int);
void is_present(const char*, int);
void disconnect(const char*);
void who(int);


#endif
