#include "str.h"


void clear_console(void)
{
	pid_t status = fork();
	switch(status)
	{
		case -1:
			printf("\n\n\033[35mCouldn\'t \'fork()\' the current process into a child: %s\033[0m\n\n", strerror(errno));
			break;

		case 0:
			if(execvp("clear", (char *const[]){"clear", NULL}) == -1)
			{
				printf("\n\n\033[35mCouldn\'t \'exec()\' the command: %s\033[0m\n\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			exit(EXIT_SUCCESS);

		default:
			wait(&status);
			break;
	}
}


void command_cursor(void)
{
	printf("|: ");
	fflush(stdout);
}


void prompt_keyboard(char *const buffer)
{
	strncpy(buffer, "", COMMANDBUFFER);
	fgets(buffer, COMMANDBUFFER, stdin);
}


void free_args(char **args, int16_t *const nbArgs)
{
	for(int16_t i = 0; i < *nbArgs; i++)
	{
		free(args[i]);
		args[i] = NULL;
	}

	*nbArgs = 0;
}


void parse_command(char *const buffer, char **const args, int16_t *const nbArgs)
{
	size_t size = 0;
	char *ptr = NULL;

	const size_t length = strlen(buffer);

	if(buffer[length - 1] == '\n')
	{
		buffer[length - 1] = '\0';
	}

	uint16_t i = 0;
	while(i < length)
	{
		while(buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\0')
		{
			if(++i >= length)
			{
				break;
			}
		}

		if(i >= length)
		{
			break;
		}

		ptr = strstr(buffer + i, " ");
		size = strlen(buffer + i) - (ptr != NULL ? strlen(ptr) : 0);
		args[*nbArgs] = strndup(buffer + i, size);

		(*nbArgs)++;
		i += size + 1;
	}
}


bool command_validation(const char *const *const args, const int16_t nbArgs, const char *const command, const uint8_t nbCommandArgs)
{
	if(strcmp(args[0], command) || nbArgs != nbCommandArgs)
	{
		return false;
	}

	else
	{
		return true;
	}
}


bool str_beginwith(const char *w, const char *s)
{
	while(*s)
	{
		if(*s != *w)
		{
			return false;
		}

		w++;
		s++;
	}

	return true;
}


bool prompt_yes_no(char *const buffer, char **const args, int16_t *const nbArgs)
{
	while(true)
	{
		command_cursor();
		prompt_keyboard(buffer);
		free_args(args, nbArgs);
		parse_command(buffer, args, nbArgs);

		if(*nbArgs == 1)
		{
			if(!strcmp(args[0], "no"))
			{
				return false;
			}

			else
			{
				if(!strcmp(args[0], "yes") || !strcmp(args[0], "YES"))
				{
					return true;
				}
			}
		}
	}
}


const char* getSecondArgsGroup(const char *const buffer)
{
	return strstr(buffer, " ") + 1;
}


int8_t getSecondArgsGroupAsInteger(const char *const buffer)
{
	return strtoul(strstr(buffer, " ") + 1, NULL, 10);
}


const char* getThirdArgsGroup(const char *const buffer)
{
	return strstr(strstr(buffer, " ") + 1, " ") + 1;
}
