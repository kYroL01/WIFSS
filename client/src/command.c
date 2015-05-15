#include <client.h>

void* scom(void *data)
{
	int _sock;
	int _result;
	char _buff[BUFFER] = {0};

	_sock = *((int*)data);

	while(1)
	{
		printf("test1\n");
		memset(_buff, 0, BUFFER);
		_result = recv(_sock, _buff, BUFFER, false);

		if(_result <= 0)
		{
			pthread_exit(NULL);
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
				pthread_exit(NULL);
			}

			else
			{
				printf("\n\n[sthread] Command unknown received from Server.\n");
			}
		}
	}

	return NULL;
}

void* clientCommand(void *data)
{
	bool _keepGoing = true;

	int _sock;

	_sock = *((int*)data);

	while(_keepGoing)
	{
		communication(_sock, &_keepGoing);
	}

	pthread_exit(NULL);
}

void handle_command(const char *command, int _sock, bool *keepGoing)
{
	if(!strcmp(command, QUIT) || !strcmp(command, EXIT) || !strcmp(command, STOP))
	{
		if(!_tunnelOpened_)
		{
			printf("\n\nLet's close connection with Server...\n");
			*keepGoing = false;
		}
		else
		{
			printf("\n\nLet's close tunnel with the other Client...\n");
			_tunnelOpened_ = false;
		}
	}

	else if(str_beginwith(command, DOWNLOAD) && str_validation(command, ARGDWL))
	{
		char _path[32] = {0};
		
		send(_sock, command, BUFFER, false);
		
		sscanf(command, "download %s", _path);		

		download(_path, _sock);
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

	else if(str_beginwith(command, SEND))
	{
		send(_sock, command, BUFFER, false);
	}

	else if(str_beginwith(command, SENDP))
	{
		if(!_tunnelOpened_)
		{
			send(_sock, command, BUFFER, false);
		}
		else
		{
			printf("You're tunneling ! You don't have to inform the recipient.\n");
		}
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

void communication(int sock, bool *booleen)
{
	static char _buff[BUFFER];

	memset(_buff, 0, BUFFER);
	
	if(!_tunnelOpened_)
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
		handle_command(_buff, sock, NULL);
	}
}