#include <client.h>

void* serverCommunication(void *param)
{
	int result;
	char buff[BUFFER];

	DATA *data = (DATA*)param;

	while(1)
	{
		memset(buff, 0, BUFFER);

		result = recv(data->sock, buff, BUFFER, false);

		if(result <= 0 || !strcmp(buff, DISCONNECT))
		{
			printf("\n\n[sthread] [Server] is demanding the Client disconnection. Stopping now.");
			pthread_cancel(*(data->cthread));
			pthread_exit(NULL);
		}

		else
		{
			static _Bool smthWritten;
			smthWritten = true;

			if(str_beginwith(buff, UPLOAD))
			{
				upload(buff, data->sock);
			}

			else if(!strcmp(buff, ASKLIST) && checkDownloadFolder())
			{
				char buff[BUFFER] = "";
				listFiles(buff);
				send(data->sock, buff, BUFFER, false);
			}

			else if(str_beginwith(buff, ISPRESENT) && checkDownloadFolder())
			{
				isPresent(buff, data->sock);
			}

			else
			{
				if(strcmp(buff, ""))
				{
					printf("\n\n[sthread] \"%s\"", buff);
				}

				else
				{
					smthWritten = false;
				}
			}

			if(smthWritten)
			{
				printf("\n\n|: ");
				fflush(stdout);
			}
		}
	}
}

void* clientCommunication(void *param)
{
	DATA *data = (DATA*)param;

	char buff[BUFFER];

	while(1)
	{
		printf("|: ");
		promptKeyboard(buff);

		if(!handle_command(buff, data))
		{
			pthread_cancel(*(data->sthread));
			pthread_exit(NULL);
		}
	}
}

_Bool handle_command(const char *command, DATA *data)
{
	if((str_beginwith(command, QUIT) && str_infiniteSpaces(command + strlen(QUIT))) || (str_beginwith(command, EXIT) && str_infiniteSpaces(command + strlen(EXIT))) || (str_beginwith(command, LOGOUT) && str_infiniteSpaces(command + strlen(LOGOUT))) || (str_beginwith(command, CLOSE) && str_infiniteSpaces(command + strlen(CLOSE))))
	{
		return false;
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

		short int i;
		for(i = 0; helpMenu[i] != NULL; i++)
		{
			printf("\t%s\n", helpMenu[i]);
		}

		printf("\n");
	}

	else
	{
		printf("\nCommand unknown. Try \"?\" or \"help\" for further information.\n\n");
	}

	return true;
}
