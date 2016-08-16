#include <server.h>

void commandCursor()
{
	printf("|: ");
	fflush(stdout);
}

void* command_handler(void* data)
{
	int *listen_socket = (int*)data;

	char buffer[BUFFER];

	while(1)
	{
		commandCursor();
		promptKeyboard(buffer);

		if((str_beginwith(buffer, QUIT) && str_infiniteSpaces(buffer + strlen(QUIT))) || (str_beginwith(buffer, EXIT) && str_infiniteSpaces(buffer + strlen(EXIT))) || (str_beginwith(buffer, STOP) && str_infiniteSpaces(buffer + strlen(STOP))) || (str_beginwith(buffer, HALT) && str_infiniteSpaces(buffer + strlen(HALT))) || (str_beginwith(buffer, CLOSE) && str_infiniteSpaces(buffer + strlen(CLOSE))))
		{
			broadcast(SID, "[Server] is going to shutdown !");
			break;
		}

		else if(str_beginwith(buffer, SEND))
		{
			char cpy[BUFFER]      = "";
			char buffTemp[BUFFER] = "";
			sscanf(buffer, "send %[^\n]", cpy);
			sprintf(buffTemp, "[Server] says: \"%s\".", cpy);
			broadcast(SID, buffTemp);
		}

		else if(str_beginwith(buffer, WHISPER))
		{
			char cpy[BUFFER]      = "";
			char buffTemp[BUFFER] = "";
			short int idTemp      = -1;
			sscanf(buffer, "whisper %hd %[^\n]", &idTemp, cpy);
			if(idTemp >= 0 && g_clients[idTemp].status == TAKEN && idTemp < MAX_CLIENTS)
			{
				sprintf(buffTemp, "[Server] whispers to you: \"%s\".", cpy);
				send(g_clients[idTemp].sock, buffTemp, BUFFER, false);
			}
			else
			{
				printf("\n[WIFSS] This client identifier is invalid.\n\n");
			}
		}

		else if(str_beginwith(buffer, DISCONNECT) && str_validation(buffer, ARGDCL) && str_infiniteSpaces(buffer + strlen(DISCONNECT)))
		{
			disconnect(buffer);
		}

		else if(str_beginwith(buffer, CLEAR) && str_infiniteSpaces(buffer + strlen(CLEAR)))
		{
			system("clear");
		}

		else if(str_beginwith(buffer, WHO) && str_infiniteSpaces(buffer + strlen(WHO)))
		{
			who(-1);
		}

		else if(str_infiniteSpaces(buffer))
		{
			/* Do nothing... */
		}

		else if((str_beginwith(buffer, HELP) && str_infiniteSpaces(buffer + strlen(HELP))) || (str_beginwith(buffer, INTERROGATIONPOINT) && strlen(INTERROGATIONPOINT)))
		{
			static const char *helpMenu[MAXCMD] =
			{
				"?",
				"help",
				"who",
				"send <message>",
				"whisper <idClient> <message>",
				"disconnect <idClient> ['-1' for all]",
				"quit",
				"exit",
				"halt",
				"stop",
				"close",
				"clear"
			};

			short int _j;
			for(_j = 0; helpMenu[_j] != NULL; _j++)
			{
				printf("\t");
				puts(helpMenu[_j]);
			}
		}

		else
		{
			printf("\nCommand unknown. Try \"?\" or \"help\" for further information.\n\n");
		}
	}

	closeServer(*listen_socket);

	exit(false);
}
