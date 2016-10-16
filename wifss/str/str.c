#include "str.h"


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


bool str_validation(const char *str, const uint8_t nbArgs)
{
	uint8_t arg = 1;

	for(uint16_t i = 0; i < BUFFER; i++)
	{
		if(str[i] == ' ' && (str[i + 1] != ' ' && str[i + 1] != '\0'))
		{
			arg++;
		}
	}

	if(arg > nbArgs)
	{
		printf("\nToo many arguments for this command.\n");
		return false;
	}

	else if(arg < nbArgs)
	{
		printf("\nToo few arguments for this command.\n");
		return false;
	}

	else
	{
		return true;
	}
}


bool str_infinite_spaces(const char *buff)
{
	int16_t length = strlen(buff);

	for(uint16_t i = 0; i < length; i++)
	{
		if(buff[i] != ' ' && buff[i] != '\t')
		{
			return false;
		}
	}

	return true;
}


void str_lower_case(char *buff)
{
	uint16_t length = strlen(buff);

	/* Lower only the first word (the command) */
	for(uint16_t i = 0; i < length && buff[i] != ' '; i++)
	{
		buff[i] = tolower(buff[i]);
	}
}


void str_remove_slash_n(char *buff)
{
	char *slashNPosition = strchr(buff, '\n');

    if(slashNPosition != NULL)
    {
		*slashNPosition = '\0';
    }
}


void prompt_keyboard(char *buff)
{
	strcpy(buff, "");
	fgets(buff, FGETSBUFF, stdin);
	str_remove_slash_n(buff);
	str_lower_case(buff);
}


bool command_validation(const char *buff, const char *command, const uint8_t nbArgs)
{
	if(str_beginwith(buff, command))
	{
		if(nbArgs == 0)
		{
			return str_infinite_spaces(buff + strlen(command));
		}

		else
		{
			return str_validation(buff, nbArgs);
		}
	}

	else
	{
		return false;
	}
}


void command_cursor(void)
{
	printf("|: ");
	fflush(stdout);
}
