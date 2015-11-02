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
	short int _i, _arg;

	for(_i = 0, _arg = 1; _i < BUFFER; _i++)
	{
		if(str[_i] == ' ' && (str[_i + 1] != ' ' && str[_i + 1] != '\0'))
		{
			_arg++;
		}
	}

	if(_arg > nbArgs)
	{
		printf("Too many arguments for this command.\n\n");
		return false;
	}

	else if(_arg < nbArgs)
	{
		printf("Too few arguments for this command.\n\n");
		return false;
	}

	return true;
}

_Bool str_infiniteSpaces(const char *buff)
{
	short int _i;
	short int _length = (short int)strlen(buff);

	for(_i = 0; _i < _length; _i++)
	{
		if(buff[_i] != ' ')
		{
			return false;
		}
	}

	return true;
}

void str_lowerCase(char *buff)
{
	short int _i;
	short int _length = (short int)strlen(buff);

	for(_i = 0; _i < _length && buff[_i] != ' '; _i++) //Tolower only first word !
	{
		buff[_i] = tolower(buff[_i]);
	}
}

void str_removeSlashN(char *buff)
{
	char *_slashNPosition = strchr(buff, '\n');

    if(_slashNPosition != NULL)
    {
		*_slashNPosition = '\0';
    }
}

void promptKeyboard(char *buff)
{
	memset(buff, 0, BUFFER);
	fgets(buff, FGETSBUFF, stdin);
	str_removeSlashN(buff);
	str_lowerCase(buff);
}
