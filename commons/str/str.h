#ifndef __STR__
#define __STR__


#include "../commons.h"


void clear_console(void);
void command_cursor(void);
void prompt_keyboard(char *const);
void free_args(char**, uint16_t *const);
void parse_command(char *const, char **const, uint16_t *const);
bool command_validation(const char *const *const, const uint16_t, const char *const, const uint8_t);
bool str_beginwith(const char*, const char*);
bool prompt_yes_no(char *const, char **const, uint16_t *const);
const char* getSecondArgsGroup(const char *const buffer);
int8_t getSecondArgsGroupAsInteger(const char *const buffer);
const char* getThirdArgsGroup(const char *const buffer);


#endif
