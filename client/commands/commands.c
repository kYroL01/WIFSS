#include "commands.h"


void* server_communication(void *param)
{
	int16_t result;
	char buff[BUFFER];
	bool newCursor = true;

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
				/* We received from server a non-null string, let's print it */
				if(strcmp(buff, ""))
				{
					printf("\n\n[sthread] \"%s\"\n\n", buff);
				}

				else
				{
					newCursor = false;
				}
			}

			if(newCursor)
			{
				command_cursor();
			}

			else
			{
				newCursor = true;
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
		command_cursor();
		prompt_keyboard(buff);

		if(command_validation(buff, EXIT, 0) || command_validation(buff, LOGOUT, 0))
		{
			pthread_cancel(*(threads->sthread));
			pthread_exit(NULL);
		}

		else if(command_validation(buff, DOWNLOAD, ARGDWL))
		{
			download(buff);
		}

		else if(str_beginwith(buff, SEND))
		{
			send(g_core_variables.server_sock, buff, BUFFER, false);
		}

		else if(command_validation(buff, WHISPER, ARGWHI))
		{
			send(g_core_variables.server_sock, buff, BUFFER, false);
		}

		else if(command_validation(buff, ISPRESENT, ARGISP))
		{
			send(g_core_variables.server_sock, buff, BUFFER, false);
		}

		else if(command_validation(buff, ASKLIST, ARGASK))
		{
			ask_list(buff);
		}

		else if(command_validation(buff, REMOVE, ARGRMV))
		{
			remove_file(buff);
		}

		else if(command_validation(buff, RENAME, ARGRNA))
		{
			rename_file(buff);
		}

		else if(command_validation(buff, LIST, 0))
		{
			list_files(NULL);
		}

		else if(command_validation(buff, WHO, 0))
		{
			char temp[BUFFER] = WHO;
			send(g_core_variables.server_sock, temp, BUFFER, false);
		}

		else if(command_validation(buff, CLEAR, 0))
		{
			system("clear");
		}

		else if(command_validation(buff, CHECKFOLDER, 0))
		{
			if(!check_download_folder())
			{
				pthread_cancel(*(threads->sthread));
				pthread_exit(NULL);
			}
		}

		else if(str_infinite_spaces(buff))
		{
			/* Do nothing... */
		}

		else if(command_validation(buff, HELP, 0) || command_validation(buff, INTERROGATIONPOINT, 0))
		{
			static const char *const helpMenu[32] =
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
				"exit",
				"logout",
				"clear",
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
	}
}
