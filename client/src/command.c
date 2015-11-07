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

			if(str_beginwith(_buff, UPLOAD))
			{
				upload(_buff, data->sock);
			}

			else if(!strcmp(_buff, DISCONNECT))
			{
				printf("\n\n[sthread] [Server] is demanding the Client disconnection. Stopping now.");
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
				fflush(stdout);
			}
		}

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
	if(!strcmp(command, QUIT) || !strcmp(command, EXIT) || !strcmp(command, STOP) || !strcmp(command, LOGOUT) || !strcmp(command, CLOSE))
	{
		data->keepGoing = false;
	}

	else if(str_beginwith(command, DOWNLOAD) && str_validation(command, ARGDWL))
	{
		download(command, data->sock);
	}

	else if(str_beginwith(command, SEND))
	{
		send(data->sock, command, BUFFER, false);
	}

	else if(str_beginwith(command, WHISPER) && str_validation(command, ARGWHI))
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
			"whisper <idClient> <message>",
			"quit",
			"exit",
			"logout",
			"clear",
			"close",
			"download <idClient> <file>"
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
