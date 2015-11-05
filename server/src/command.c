#include <server.h>

void* command_handler(void* data)
{
	int *listen_socket = (int*)data;

	char _buffer[BUFFER];
	char _cpy[BUFFER];

	while(1)
	{
		commandCursor();
		promptKeyboard(_buffer);

		if(!strcmp(_buffer, QUIT) || !strcmp(_buffer, STOP)|| !strcmp(_buffer, HALT) || !strcmp(_buffer, EXIT) || !strcmp(_buffer, CLOSE))
		{
			broadcast(SID, "[Server] is going to shutdown !");
			break;
		}

		else if(str_beginwith(_buffer, SEND))
		{
			memset(_cpy, 0, BUFFER);
			char _buffTemp[BUFFER] = "";
			sscanf(_buffer, "send %[^\n]", _cpy);
			sprintf(_buffTemp, "[Server] says: \"%s\".", _cpy);
			broadcast(SID, _buffTemp);
		}

		else if(str_beginwith(_buffer, SENDP))
		{
			memset(_cpy, 0, BUFFER);
			char _buffTemp[BUFFER] = "";
			short int _idTemp = -1;
			sscanf(_buffer, "sendp %hd %[^\n]", &_idTemp, _cpy);
			if(g_clients[_idTemp].status == TAKEN &&_idTemp >= 0 && _idTemp < MAX_CLIENTS)
			{
				sprintf(_buffTemp, "[Server] whispers to you: \"%s\".", _cpy);
				send(g_clients[_idTemp].sock, _buffTemp, BUFFER, false);
			}
			else
			{
				printf("\n[WIFSS] This client identifier is invalid.\n\n");
			}
		}

		else if(str_beginwith(_buffer, DISCONNECT) && str_validation(_buffer, ARGDCL))
		{
			disconnect(_buffer);
		}

		else if(!strcmp(_buffer, CLEAR))
		{
			system("clear");
		}

		else if(str_infiniteSpaces(_buffer))
		{
			//Do nothing...
		}

		else if(!strcmp(_buffer, HELP) || !strcmp(_buffer, INTERROGATIONPOINT))
		{
			static const char *helpMenu[MAXCMD] =
			{
				"?",
				"help",
				"send <message>",
				"sendp <idClient> <message>",
				"disconnect <idClient> ['-1' for all]",
				"quit",
				"exit",
				"halt",
				"stop",
				"close",
				"clear"
			};

			for(short int _j = 0; helpMenu[_j] != NULL; _j++)
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
