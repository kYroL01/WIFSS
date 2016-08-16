#include <client.h>

_Bool str_beginwith(const char *w, const char *s)
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

_Bool str_validation(const char *str, short int nbArgs)
{
	short int i, arg;

	for(i = 0, arg = 1; i < BUFFER; i++)
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

	return true;
}

_Bool str_infiniteSpaces(const char *buff)
{
	short int i;
	short int length = (short int)strlen(buff);

	for(i = 0; i < length; i++)
	{
		if(buff[i] != ' ' && buff[i] != '\t')
		{
			return false;
		}
	}

	return true;
}

void str_lowerCase(char *buff)
{
	short int i;
	short int length = (short int)strlen(buff);

	for(i = 0; i < length && buff[i] != ' '; i++) /* Tolower only first word ! */
	{
		buff[i] = tolower(buff[i]);
	}
}

void str_removeSlashN(char *buff)
{
	char *slashNPosition = strchr(buff, '\n');

    if(slashNPosition != NULL)
    {
		*slashNPosition = '\0';
    }
}

void promptKeyboard(char *buff)
{
	memset(buff, 0, BUFFER);
	fgets(buff, FGETSBUFF, stdin);
	str_removeSlashN(buff);
	str_lowerCase(buff);
}
