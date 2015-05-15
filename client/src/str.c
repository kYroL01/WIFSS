#include <client.h>

int str_beginwith(const char *w, const char *s)
{
	while(*s)
	{
		if(*s != *w)
		{
			return 0;
		}

		w++;
		s++;
	}

	return 1;
}

int str_validation(const char *str, short int nbArgs)
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
		return 0;
	}

	else if(_arg < nbArgs)
	{
		printf("Too few arguments for this command.\n\n");
		return 0;
	}

	return 1;
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
