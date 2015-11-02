#include <client.h>

void* serverCommunication(void *param)
{
	int _result;
	char _buff[BUFFER];

	MUTEX *data = (MUTEX*)param;

	while(1)
	{
		memset(_buff, 0, BUFFER);

		_result = recv(data->sock, _buff, BUFFER, false);

		//pthread_mutex_lock(&(data->mutex));

		if(_result <= 0 || !data->keepGoing)
		{
			data->keepGoing = false;
			//pthread_mutex_unlock(&(data->mutex));
			pthread_exit(NULL);
		}

		else
		{
			static _Bool _someThingWritten;
			_someThingWritten = true;

			if(str_beginwith(_buff, UPLOAD) && str_validation(_buff, ARGUPL))
			{
				char _path[PATHSIZE] = {0};
				sscanf(_buff, "upload %s", _path);
				printf("\n\n[sthread] Server is asking us to upload: \"%s\".", _path);
				upload(_path, data->sock);
			}

			else if(str_beginwith(_buff, ASKTUNNEL) && str_validation(_buff, ARGTUN))
			{
				int _clientAsking = 0;
				sscanf(_buff, "asktunnel %d", &_clientAsking);

				if(!data->tunnelOpened)
				{
					acceptunnel(data, _clientAsking);
				}
				else
				{
					printf("\n\n[sthread] Client %d is asking you for a tunnel but you're already tunneled.", _clientAsking);
				}

			}

			else if(!strcmp(_buff, DISCONNECT))
			{
				printf("\n\n[sthread] Server is demanding the Client disconnection. Stopping now.");
				data->keepGoing = false;
				break;
			}

			else
			{
				if(strcmp(_buff, ""))
				{
					printf("\n\n[sthread] \"%s\"", _buff);
				}
				else
				{
					_someThingWritten = false;
				}
			}

			if(_someThingWritten)
			{
				printf("\n\n|: ");
			}
		}

		fflush(stdout);

		//pthread_mutex_unlock(&(data->mutex));
	}

	pthread_exit(NULL);
}


void* clientCommunication(void *param)
{
	MUTEX *data = (MUTEX*)param;

	char _buff[BUFFER];

	while(1)
	{
		//pthread_mutex_lock(&(data->mutex));

		if(!data->keepGoing)
		{
			break;
		}

		if(data->tunnelOpened)
		{
			printf("[Tunnel] ");
		}

		printf("|: ");
		promptKeyboard(_buff);
		handle_command(_buff, data);

		//pthread_mutex_unlock(&(data->mutex));
	}

	//pthread_mutex_unlock(&(data->mutex));
	pthread_exit(NULL);
}


void* infiniteWaitingFnct(void *param)
{
	MUTEX *data = (MUTEX*)param;

	for(; ; sleep(1))
	{
		//pthread_mutex_lock(&(data->mutex));
		if(!data->keepGoing)
		{
			break;
		}
		//pthread_mutex_unlock(&(data->mutex));
	}

	//pthread_mutex_unlock(&(data->mutex));
	pthread_exit(NULL);
}


void handle_command(const char *command, MUTEX *data)
{
	if(!strcmp(command, QUIT) || !strcmp(command, EXIT) || !strcmp(command, STOP) || !strcmp(command, LOGOUT))
	{
		if(!data->tunnelOpened)
		{
			printf("\n\n[WIFSS] Let's close connection with Server...");
			data->keepGoing = false;
		}
		else
		{
			printf("\n\n[WIFSS] Let's close tunnel with the other Client...\n");
			data->tunnelOpened = false;
		}
	}

	else if(str_beginwith(command, DOWNLOAD) && str_validation(command, ARGDWL))
	{
		char _path[PATHSIZE] = {0};
		send(data->sock, command, BUFFER, false);
		sscanf(command, "download %s", _path);		
		download(_path, data->sock);
	}

	else if(str_beginwith(command, TUNNEL) && str_validation(command, ARGTUN))
	{
		if(!data->tunnelOpened)
		{
			int _idClient = 0;
			sscanf(command, "tunnel %d", &_idClient);
			startunnel(data, _idClient);
		}
		else
		{
			printf("[WIFSS] You're already tunneled with someone. {TIPS} Type \"logout\" to stop the tunnel.\n\n");
		}
	}

	else if(str_beginwith(command, SENDP))
	{
		if(!data->tunnelOpened)
		{
			send(data->sock, command, BUFFER, false);
		}
		else
		{
			printf("[WIFSS] {TIPS} You're tunneling ! You don't have to inform the recipient.\n");
		}
	}

	else if(str_beginwith(command, SEND))
	{
		send(data->sock, command, BUFFER, false);
	}


	else if(!strcmp(command, CLEAR))
	{
		system("clear");
	}

	else if(str_infiniteSpaces(command))
	{
		//Do nothing...
	}

	else if(!strcmp(command, HELP) || !strcmp(command, INTERROGATIONPOINT))
	{
		static const char *helpMenu[TALLERCMD] =
		{
			"?",
			"help",
			"send <message>",
			"sendp <idClient> <message>",
			"quit",
			"exit",
			"logout",
			"clear",
			"tunnel <idClient>",
			"download <file> <idClient>"
		};
		
		for(short int _i = 0; helpMenu[_i] != NULL; _i++)
		{
			printf("\t");
			puts(helpMenu[_i]);
		}
	}

	else
	{
		printf("\nCommand unknown. Try \"?\" or \"help\" for further information.\n\n");
	}
}
