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

void* scom(void *data)
{
	int _sock;
	int _result;
	char _buff[BUFFER] = {0};

	_sock = *((int*)data);

	_sComOn_ = true;
	while(_sComOn_)
	{
		memset(_buff, 0, BUFFER);
		_result = recv(_sock, _buff, BUFFER, 0);

		if(_result <= 0)
		{
			_sComOn_ = false;
		}

		else
		{			
			printf("\n\n[sthread] Received from server: %s\n", _buff);

			if(str_beginwith(_buff, UPLOAD) && str_validation(_buff, ARGUPL))
			{
				char _path[BUFFER] = {0};
				sscanf(_buff, "upload %s", _path);
				printf("\n\n[sthread] Server is asking us to upload: %s\n", _path);
				upload(_path, _sock);
			}

			else if(str_beginwith(_buff, ASKTUNNEL) && str_validation(_buff, ARGTUN))
			{
				if(!_tunnelOpened_)
				{
					int _clientAsking = 0;

					sscanf(_buff, "asktunnel %d", &_clientAsking);

					acceptunnel(_sock, _clientAsking);
				}
				else
				{
					printf("\n\n[sthread] Someone is asking you for a tunnel but you're already tunneled\n");
				}
			}

			else if(!strcmp(_buff, DISCONNECT))
			{
				printf("\n\n[sthread] Server is demanding the Client disconnection. Stopping now.\n");
				_sComOn_ = false;
			}

			else
			{
				printf("\n\n[sthread] Command unknown received from Server.\n");
			}
		}
	}

	return NULL;
}

void handle_command(const char *command, int _sock, bool *keepGoing)
{
	if(!strcmp(command, QUIT) || !strcmp(command, EXIT))
	{
		printf("\n\nLet's close this connection...");
		*keepGoing = false;
	}

	else if(str_beginwith(command, DOWNLOAD) && str_validation(command, ARGDWL))
	{
		char _path[32] = {0};

		send(_sock, command, BUFFER, false);

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
		if(!_tunnelOpened_)
		{
			int _idClient = 0;

			sscanf(command, "tunnel %d", &_idClient);

			startunnel(_sock, _idClient);
		}
		else
		{
			printf("You're already tunneled with someone.\n\n");
		}
	}

	else if(str_beginwith(command, SEND) || str_beginwith(command, SENDP))
	{
		send(sock, _buff, BUFFER, false);
	}

	else if(!strcmp(command, CLEAR))
	{
		system("clear");
	}

	else if(!strcmp(command, "\0") || !strcmp(command, " "))
	{
		//Do nothing...
	}

	else
	{
		printf("Command unknown.\n\n");
	}
}

void communication(int sock, bool *booleen, bool isTunnel)
{
	static char _buff[BUFFER];

	memset(_buff, 0, BUFFER);
	
	if(!isTunnel)
	{
		printf("|: ");

		gets(_buff);

		str_lowerCase(_buff);

		handle_command(_buff, sock, booleen);
	}

	else
	{
		printf("[Tunnel] |: ");

		gets(_buff);

		str_lowerCase(_buff);

		sprintf(_buff, "%s: %s", ITSATUN, _buff);

		handle_command(_buff, sock, booleen);
	}
}