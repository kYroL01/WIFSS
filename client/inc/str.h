#ifndef __STR__
#define __STR__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


_Bool str_beginwith(const char*, const char*);
_Bool str_validation(const char*, short int);
_Bool str_infiniteSpaces(const char*);
void str_lowerCase(char*);
void str_removeSlashN(char*);
void promptKeyboard(char*);


#endif
