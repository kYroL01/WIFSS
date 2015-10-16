#include <client.h>

void* serverCommunication(void *param)
{
	int _result;
	char _buff[BUFFER] = {0};

	MUTEX data = *((MUTEX*)param);

	while(1)
	{
		pthread_mutex_lock(&(data.mutex));

		memset(_buff, 0, BUFFER);

		_result = recv(data.sock, _buff, BUFFER, false);
		if(_result <= 0 || !data.keepGoing)
		{
			data.keepGoing = false;
			pthread_mutex_unlock(&(data.mutex));
			pthread_exit(NULL);
		}

		else
		{
			/* printf("\n\n[sthread] Received from server: %s\n", _buff); */

			if(str_beginwith(_buff, UPLOAD) && str_validation(_buff, ARGUPL))
			{
				char _path[PATHSIZE] = {0};
				sscanf(_buff, "upload %s", _path);
				printf("\n\n[sthread] Server is asking us to upload: %s\n\n:|", _path);
				upload(_path, data.sock);
			}

			else if(str_beginwith(_buff, ASKTUNNEL) && str_validation(_buff, ARGTUN))
			{
				if(!data.tunnelOpened)
				{
					int _clientAsking = 0;

					sscanf(_buff, "asktunnel %d", &_clientAsking);

					acceptunnel(&data, _clientAsking);
				}
				else
				{
					printf("\n\n[sthread] Someone is asking you for a tunnel but you're already tunneled.\n\n:|");
				}
			}

			else if(!strcmp(_buff, DISCONNECT))
			{
				printf("\n\n[sthread] Server is demanding the Client disconnection. Stopping now.\n");
				data.keepGoing = false;
				pthread_mutex_unlock(&(data.mutex));
				pthread_exit(NULL);
			}

			else
			{
				printf("\n\n[sthread] Command unknown received from Server.\n\n:|");
			}
		}

		pthread_mutex_unlock(&(data.mutex));
	}

	pthread_exit(NULL);
}

void* clientCommunication(void *param)
{
	MUTEX data = *((MUTEX*)param);

	while(1)
	{
		pthread_mutex_lock(&(data.mutex));

		if(!data.keepGoing)
		{
			pthread_mutex_unlock(&(data.mutex));
			pthread_exit(NULL);
		}

		communication(&data);

		pthread_mutex_unlock(&(data.mutex));
	}

	pthread_exit(NULL);
}

void handle_command(const char *command, MUTEX *data)
{
	if(!strcmp(command, QUIT) || !strcmp(command, EXIT) || !strcmp(command, STOP) || !strcmp(command, LOGOUT))
	{
		if(!data->tunnelOpened)
		{
			printf("\n\n[WIFFS] Let's close connection with Server...");
			data->keepGoing = false;
		}
		else
		{
			printf("\n\n[WIFFS] Let's close tunnel with the other Client...\n");
			data->tunnelOpened = false;
		}

		sleep(1);
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
			printf("[WIFFS] You're already tunneled with someone.\n\n");
		}
	}

	else if(str_beginwith(command, SEND))
	{
		send(data->sock, command, BUFFER, false);
	}

	else if(str_beginwith(command, SENDP))
	{
		if(!data->tunnelOpened)
		{
			send(data->sock, command, BUFFER, false);
		}
		else
		{
			printf("[WIFFS] You're tunneling ! You don't have to inform the recipient.\n");
		}
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
			"download <file> <idClient>"
		};

		//short int _size = (short int)sizeof(helpMenu) / (TALLERCMD * sizeof(**helpMenu)) + 1;
		
		for(short int _i = 0; helpMenu[_i] != NULL; _i++)
		{
			printf("\t");
			puts(helpMenu[_i]);
		}
	}

	else
	{
		printf("Command unknown.\n\n");
	}
}

void communication(MUTEX *data)
{
	static char _buff[BUFFER];

	memset(_buff, 0, BUFFER);

	if(!data->tunnelOpened)
	{
		printf("|: ");
	}

	else
	{
		printf("[Tunnel] |: ");
	}

	fgets(_buff, FGETSBUFF, stdin);
	str_removeSlashN(_buff);
	str_lowerCase(_buff);
	handle_command(_buff, data);
}
