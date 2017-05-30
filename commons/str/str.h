#ifndef __STR__
#define __STR__


#include "../commons.h"


void clear_console(void);
void command_cursor(void);
void prompt_keyboard(char *const);
void free_args(char**, int16_t *const);
void parse_command(char *const, char **const, int16_t *const);
bool command_validation(const char *const *const, const int16_t, const char *const, const uint8_t);
bool str_beginwith(const char*, const char*);
bool prompt_yes_no(char *const, char **const, int16_t *const);
const char* get_second_args_group(const char *const);
int16_t get_second_args_group_as_integer(const char *const);
const char* get_third_arg_group(const char *const);


#endif
