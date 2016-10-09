#include "commands.h"


void* server_communication(void *param)
{
	int16_t result;
	char buff[BUFFER];

	THREADS *threads = (THREADS*)param;

	while(1)
	{
		strcpy(buff, "");

		result = recv(g_core_variables.server_sock, buff, BUFFER, false);

		if(result <= 0 || !strcmp(buff, DISCONNECT))
		{
			printf("\n\n[sthread] [Server] is demanding the Client disconnection. Stopping now.");
			pthread_cancel(*(threads->cthread));
			pthread_exit(NULL);
		}

		else
		{
			static bool smthWritten;
			smthWritten = true;

			if(str_beginwith(buff, UPLOAD))
			{
				upload(buff);
			}

			else if(!strcmp(buff, ASKLIST) && check_download_folder())
			{
				char temp[BUFFER] = "";
				list_files(temp);
				send(g_core_variables.server_sock, temp, BUFFER, false);
			}

			else if(str_beginwith(buff, ISPRESENT) && check_download_folder())
			{
				is_present(buff);
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

void* client_communication(void *param)
{
	THREADS *threads = (THREADS*)param;

	char buff[BUFFER];

	while(1)
	{
		printf("|: ");
		prompt_keyboard(buff);

		if(!handle_command(buff))
		{
			pthread_cancel(*(threads->sthread));
			pthread_exit(NULL);
		}
	}
}

bool handle_command(const char *command)
{
	if((str_beginwith(command, QUIT) && str_infinite_spaces(command + strlen(QUIT))) || (str_beginwith(command, EXIT) && str_infinite_spaces(command + strlen(EXIT))) || (str_beginwith(command, LOGOUT) && str_infinite_spaces(command + strlen(LOGOUT))) || (str_beginwith(command, CLOSE) && str_infinite_spaces(command + strlen(CLOSE))))
	{
		return false;
	}

	else if(str_beginwith(command, DOWNLOAD) && str_validation(command, ARGDWL))
	{
		download(command);
	}

	else if(str_beginwith(command, SEND))
	{
		send(g_core_variables.server_sock, command, BUFFER, false);
	}

	else if(str_beginwith(command, WHISPER) && str_validation(command, ARGWHI))
	{
		send(g_core_variables.server_sock, command, BUFFER, false);
	}

	else if(str_beginwith(command, ISPRESENT) && str_validation(command, ARGISP))
	{
		send(g_core_variables.server_sock, command, BUFFER, false);
	}

	else if(str_beginwith(command, ASKLIST) && str_validation(command, ARGASK))
	{
		ask_list(command);
	}

	else if(str_beginwith(command, REMOVE) && str_validation(command, ARGRMV))
	{
		remove_file(command);
	}

	else if(str_beginwith(command, RENAME) && str_validation(command, ARGRNA))
	{
		rename_file(command);
	}

	else if(str_beginwith(command, LIST) && str_infinite_spaces(command + strlen(LIST)))
	{
		list_files(NULL);
	}

	else if(str_beginwith(command, WHO) && str_infinite_spaces(command + strlen(WHO)))
	{
		who();
	}

	else if(str_beginwith(command, CLEAR) && str_infinite_spaces(command + strlen(CLEAR)))
	{
		system("clear");
	}

	else if(str_beginwith(command, CHECKFOLDER) && str_infinite_spaces(command + strlen(CHECKFOLDER)))
	{
		check_download_folder();
	}

	else if(str_infinite_spaces(command))
	{
		/* Do nothing... */
	}

	else if((str_beginwith(command, HELP) && str_infinite_spaces(command + strlen(HELP))) || (str_beginwith(command, INTERROGATIONPOINT) && strlen(INTERROGATIONPOINT)))
	{
		static const char *const helpMenu[TALLERCMD] =
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

		for(uint8_t i = 0; helpMenu[i] != NULL; i++)
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
