#ifndef __PROTOCOLS__
#define __PROTOCOLS__


#include "../core/server.h"


void download(const char*, const uint8_t);
void who(const int8_t);
void ask_list(const char*, const uint8_t);
void is_present(const char*, const uint8_t);
void message(const char*, const uint8_t);
void whisper(const char*, const uint8_t);
void broadcast(const uint8_t, const char*);
void disconnect(const char*);
void close_all_connections(void);


#endif
