#ifndef __PROTOCOLS__
#define __PROTOCOLS__


#include "../core/server.h"


void who(const int8_t);
void message(const char*, const uint8_t);
void whisper(const char*, const uint8_t);
void broadcast(const uint8_t, const char*);
void disconnect(const char*);
void close_all_connections(void);


#endif
