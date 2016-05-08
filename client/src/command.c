#include <client.h>

void* serverCommunication(void *param)
{
	int _result;
	char _buff[BUFFER];

	DATA *data = (DATA*)param;

	while(1)
	{
		memset(_buff, 0, BUFFER);

		_result = recv(data->sock, _buff, BUFFER, false);

		/* pthread_mutex_lock(&(data->mutex)); */

		if(_result <= 0 || !data->keepGoing)
		{
			data->keepGoing = false;
			/* pthread_mutex_unlock(&(data->mutex)); */
			pthread_exit(NULL);
		}

		else
		{
			static _Bool _smthWritten;
			_smthWritten = true;

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

			else if(!strcmp(_buff, ASKLIST) && checkDownloadFolder())
			{
				char _buff[BUFFER] = "";
				listFiles(_buff);
				send(data->sock, _buff, BUFFER, false);
			}

			else if(str_beginwith(_buff, ISPRESENT) && checkDownloadFolder())
			{
				isPresent(_buff, data->sock);
			}

			else
			{
				if(strcmp(_buff, ""))
				{
					printf("\n\n[sthread] \"%s\"", _buff);
				}
				else
				{
					_smthWritten = false;
				}
			}

			if(_smthWritten)
			{
				printf("\n\n|: ");
				fflush(stdout);
			}
		}

		/* pthread_mutex_unlock(&(data->mutex)); */
	}

	pthread_exit(NULL);
}


void* clientCommunication(void *param)
{
	DATA *data = (DATA*)param;

	char _buff[BUFFER];

	while(1)
	{
		/* pthread_mutex_lock(&(data->mutex)); */

		if(!data->keepGoing)
		{
			break;
		}

		printf("|: ");
		promptKeyboard(_buff);
		handle_command(_buff, data);

		/* pthread_mutex_unlock(&(data->mutex)); */
	}

	/* pthread_mutex_unlock(&(data->mutex)); */
	pthread_exit(NULL);
}


void* infiniteWaitingFnct(void *param)
{
	DATA *data = (DATA*)param;

	for(; ; sleep(1))
	{
		/* pthread_mutex_lock(&(data->mutex)); */
		if(!data->keepGoing)
		{
			break;
		}
		/* pthread_mutex_unlock(&(data->mutex)); */
	}

	/* pthread_mutex_unlock(&(data->mutex)); */
	pthread_exit(NULL);
}


void handle_command(const char *command, DATA *data)
{
	if((str_beginwith(command, QUIT) && str_infiniteSpaces(command + strlen(QUIT))) || (str_beginwith(command, EXIT) && str_infiniteSpaces(command + strlen(EXIT))) || (str_beginwith(command, LOGOUT) && str_infiniteSpaces(command + strlen(LOGOUT))) || (str_beginwith(command, CLOSE) && str_infiniteSpaces(command + strlen(CLOSE))))
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

	else if(str_beginwith(command, ISPRESENT) && str_validation(command, ARGISP))
	{
		send(data->sock, command, BUFFER, false);
	}

	else if(str_beginwith(command, ASKLIST) && str_validation(command, ARGASK))
	{
		askList(command, data->sock);
	}

	else if(str_beginwith(command, REMOVE) && str_validation(command, ARGRMV))
	{
		removeFile(command);
	}

	else if(str_beginwith(command, RENAME) && str_validation(command, ARGRNA))
	{
		renameFile(command);
	}

	else if(str_beginwith(command, LIST) && str_infiniteSpaces(command + strlen(LIST)))
	{
		listFiles(NULL);
	}

	else if(str_beginwith(command, WHO) && str_infiniteSpaces(command + strlen(WHO)))
	{
		who(data->sock);
	}

	else if(str_beginwith(command, CLEAR) && str_infiniteSpaces(command + strlen(CLEAR)))
	{
		system("clear");
	}

	else if(str_beginwith(command, CHECKFOLDER) && str_infiniteSpaces(command + strlen(CHECKFOLDER)))
	{
		checkDownloadFolder();
	}

	else if(str_infiniteSpaces(command))
	{
		/* Do nothing... */
	}

	else if((str_beginwith(command, HELP) && str_infiniteSpaces(command + strlen(HELP))) || (str_beginwith(command, INTERROGATIONPOINT) && strlen(INTERROGATIONPOINT)))
	{
		static const char *helpMenu[TALLERCMD] =
		{
			"?",
			"help",
			"who",
			"send <message>",
			"whisper <idClient> <message>",
			"list",
			"rename <file> <newFileName>",
			"remove <file>",
			"ispresent <idClient> <file>",
			"asklist <idClient>",
			"quit",
			"exit",
			"logout",
			"clear",
			"close",
			"download <idClient> <file>",
			"checkfolder"
		};

		short int _i;
		for(_i = 0; helpMenu[_i] != NULL; _i++)
		{
			printf("\t%s\n", helpMenu[_i]);
		}

		printf("\n");
	}

	else
	{
		printf("\nCommand unknown. Try \"?\" or \"help\" for further information.\n\n");
	}
}
