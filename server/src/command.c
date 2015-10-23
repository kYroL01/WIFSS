#include <server.h>

void* command_handler(void* data)
{
	char _buffer[BUFFER];
	char _cpy[BUFFER];

	while(1)
	{
		memset(_buffer, 0, BUFFER);

		printf("|: ");
		promptKeyboard(_buffer);

		if(!strcmp(_buffer, QUIT) || !strcmp(_buffer, STOP)|| !strcmp(_buffer, HALT) || !strcmp(_buffer, EXIT) || !strcmp(_buffer, CLOSE))
		{
			broadcast(SID, "Server is going to shutdown.");
			close_all_connections();
			for(short int _i = 0; _i < MAX_CLIENTS; _i++)
			{
				pthread_cancel(threads[_i]);
			}
			break;
		}

		else if(str_beginwith(_buffer, SENDP))
		{
			char _buffTemp[BUFFER] = "";
			short int _idTemp = -1;
			sscanf(_buffer, "sendp %hd %[^\n]", &_idTemp, _cpy);
			sprintf(_buffTemp, "[Server] says to you: \"%s\".", _cpy);
			send(g_clients[_idTemp].sock, _buffTemp, BUFFER, false);
		}

		else if(str_beginwith(_buffer, SEND))
		{
			memset(_cpy, 0, BUFFER);
			char _buffTemp[BUFFER] = "";
			sscanf(_buffer, "send %[^\n]", _cpy);
			sprintf(_buffTemp, "[Server] says: \"%s\".", _cpy);
			broadcast(SID, _buffTemp);
		}

		else if(str_beginwith(_buffer, DISCONNECT))
		{
			short int _idTemp = -2;
			sscanf(_buffer, "disconnect %hd", &_idTemp);
			if(_idTemp == -1)
			{
				close_all_connections(); 
				printf("\n");
			}
			else if(_idTemp >= 0 && _idTemp < MAX_CLIENTS)
			{
				if(g_clients[_idTemp].status == TAKEN)
				{
					send(g_clients[_idTemp].sock, "Server wants your disconnection.", BUFFER, false);
					send(g_clients[_idTemp].sock, DISCONNECT, BUFFER, false);
					close(g_clients[_idTemp].sock);
				}
				else
				{
					printf("This client is offline yet. Can't disconnect him.\n");
				}
			}
			else
			{
				printf("Client id is invalid.\n");
			}
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
			static const char *helpMenu[TALLERCMD] =
			{
				"?",
				"help",
				"send <message>",
				"sendp <idClient> <message>",
				"disconnect <idClient> ['-1' = all]",
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
			printf("Command unknown. Try \"?\" or \"help\" for further information.\n\n");
		}
	}
	
	printf("[WIFSS] Server stopped.\n");
	for(short int _k = 0; _k < 60; _k++)
	{
		printf("=");
	}
	printf("\n\n");
	exit(false);
	return data;
}
