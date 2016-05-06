#include <server.h>

void* command_handler(void* data)
{
	int *listen_socket = (int*)data;

	char _buffer[BUFFER];

	while(1)
	{
		commandCursor();
		promptKeyboard(_buffer);

		if((str_beginwith(_buffer, QUIT) && str_infiniteSpaces(_buffer + strlen(QUIT))) || (str_beginwith(_buffer, EXIT) && str_infiniteSpaces(_buffer + strlen(EXIT))) || (str_beginwith(_buffer, STOP) && str_infiniteSpaces(_buffer + strlen(STOP))) || (str_beginwith(_buffer, HALT) && str_infiniteSpaces(_buffer + strlen(HALT))) || (str_beginwith(_buffer, CLOSE) && str_infiniteSpaces(_buffer + strlen(CLOSE))))
		{
			broadcast(SID, "[Server] is going to shutdown !");
			break;
		}

		else if(str_beginwith(_buffer, SEND))
		{
			char _cpy[BUFFER] = "";
			char _buffTemp[BUFFER] = "";
			sscanf(_buffer, "send %[^\n]", _cpy);
			sprintf(_buffTemp, "[Server] says: \"%s\".", _cpy);
			broadcast(SID, _buffTemp);
		}

		else if(str_beginwith(_buffer, WHISPER))
		{
			char _cpy[BUFFER] = "";
			char _buffTemp[BUFFER] = "";
			short int _idTemp      = -1;
			sscanf(_buffer, "whisper %hd %[^\n]", &_idTemp, _cpy);
			if(_idTemp >= 0 && g_clients[_idTemp].status == TAKEN && _idTemp < MAX_CLIENTS)
			{
				sprintf(_buffTemp, "[Server] whispers to you: \"%s\".", _cpy);
				send(g_clients[_idTemp].sock, _buffTemp, BUFFER, false);
			}
			else
			{
				printf("\n[WIFSS] This client identifier is invalid.\n\n");
			}
		}

		else if(str_beginwith(_buffer, DISCONNECT) && str_validation(_buffer, ARGDCL) && str_infiniteSpaces(_buffer + strlen(DISCONNECT)))
		{
			disconnect(_buffer);
		}

		else if(str_beginwith(_buffer, CLEAR) && str_infiniteSpaces(_buffer + strlen(CLEAR)))
		{
			system("clear");
		}

		else if(str_infiniteSpaces(_buffer))
		{
			/* Do nothing... */
		}

		else if((str_beginwith(_buffer, HELP) && str_infiniteSpaces(_buffer + strlen(HELP))) || (str_beginwith(_buffer, INTERROGATIONPOINT) && strlen(INTERROGATIONPOINT)))
		{
			static const char *helpMenu[MAXCMD] =
			{
				"?",
				"help",
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
