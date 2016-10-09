#ifndef __STR__
#define __STR__


#include "../wifss.h"


bool str_beginwith(const char*, const char*);
bool str_validation(const char*, uint8_t);
bool str_infinite_spaces(const char*);
void str_lower_case(char*);
void str_remove_slash_n(char*);
void prompt_keyboard(char*);


#endif
