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
		printf("\nToo many arguments for this command.\n\n");
		return 0;
	}

	else if (_arg < nbArgs)
	{
		printf("\nToo few arguments for this command.\n\n");
		return 0;
	}

	else
	{
		return 1;
	}
}

void str_lowerCase(char *_buff)
{
	short int _i;
	short int length = (short int)strlen(_buff);

	for(_i = 0; _i < length; _i++)
	{
		_buff[_i] = tolower(_buff[_i]);
	}
}

void* scom(void *data)
{
	int _sock;
	int _result;
	char _buff[BUFFER] = {0};

	_sock = *((int*)data);

	bool _keepGoing = true;
	while(_keepGoing)
	{
		memset(_buff, 0, BUFFER);
		_result = recv(_sock, _buff, BUFFER, 0);

		if(_result <= 0)
		{
			_keepGoing = false;
		}

		else
		{
			if(str_beginwith(_buff, UPLOAD))
			{
				char _path[BUFFER] = {0};
				sscanf(_buff, "upload %s", _path);
				printf("Server is asking us to upload: %s\n", _path);
				upload(_path, _sock);
			}

			printf("[sthread] received from server: %s\n", _buff);
		}
	}

	return NULL;
}

void handle_command(const char *command, int _sock, bool *connected)
{
	if(!strcmp(command, QUIT))
	{
		printf("\n\nLet's close this connection...");
		*connected = false;
	}

	else if(str_beginwith(command, DOWNLOAD) && str_validation(command, ARGDWL))
	{
		char _path[32] = {0};

		sscanf(command, "download %s", _path);

		if(!download(command, _sock))
		{
			printf("File couldn't be downloaded correctly.\n\n");
		}
		else
		{
			printf("File downloaded !\n\n");
		}
	}

	else if(str_beginwith(command, TUNNEL) && str_validation(command, ARGTUN))
	{
		int idClient = 0;

		sscanf(command, "tunnel %d", &idClient);

		start_tunnel(_sock, idClient);
	}

	else
	{
		printf("\nCommand unknown.\n");
	}
}
