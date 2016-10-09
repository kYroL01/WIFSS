#include "commands.h"


void command_cursor(void)
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
		command_cursor();
		prompt_keyboard(buffer);

		if((str_beginwith(buffer, QUIT) && str_infinite_spaces(buffer + strlen(QUIT))) || (str_beginwith(buffer, EXIT) && str_infinite_spaces(buffer + strlen(EXIT))) || (str_beginwith(buffer, STOP) && str_infinite_spaces(buffer + strlen(STOP))) || (str_beginwith(buffer, HALT) && str_infinite_spaces(buffer + strlen(HALT))) || (str_beginwith(buffer, CLOSE) && str_infinite_spaces(buffer + strlen(CLOSE))))
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

		else if(str_beginwith(buffer, DISCONNECT) && str_validation(buffer, ARGDCL) && str_infinite_spaces(buffer + strlen(DISCONNECT)))
		{
			disconnect(buffer);
		}

		else if(str_beginwith(buffer, CLEAR) && str_infinite_spaces(buffer + strlen(CLEAR)))
		{
			system("clear");
		}

		else if(str_beginwith(buffer, WHO) && str_infinite_spaces(buffer + strlen(WHO)))
		{
			who(-1);
		}

		else if(str_infinite_spaces(buffer))
		{
			/* Do nothing... */
		}

		else if((str_beginwith(buffer, HELP) && str_infinite_spaces(buffer + strlen(HELP))) || (str_beginwith(buffer, INTERROGATIONPOINT) && strlen(INTERROGATIONPOINT)))
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

			for(uint8_t j = 0; helpMenu[j] != NULL; j++)
			{
				printf("\t%s\n", helpMenu[j]);
			}

			printf("\n");
		}

		else
		{
			printf("\nCommand unknown. Try \"?\" or \"help\" for further information.\n\n");
		}
	}

	close_server(*listen_socket);

	exit(false);
}


void process_command(const char *command, int sender_id)
{
	if(str_beginwith(command, DOWNLOAD))
	{
		download(command, sender_id);
	}

	else if(str_beginwith(command, WHO))
	{
		who(sender_id);
	}

	else if(str_beginwith(command, QUIT))
	{
		close(g_clients[sender_id].sock);
		return;
	}

	else if(str_beginwith(command, SEND))
	{
		message(command, sender_id);
	}

	else if(str_beginwith(command, WHISPER))
	{
		whisper(command, sender_id);
	}

	else if(str_beginwith(command, ASKLIST))
	{
		ask_list(command, sender_id);
	}

	else if(str_beginwith(command, ISPRESENT))
	{
		is_present(command, sender_id);
	}

	else
	{
		printf("\n\n[WIFSS] Unknown command from [Client %d]: \"%s\".\n\n", sender_id, command);
	}

	command_cursor();
}


void* on_connection(void *data)
{
	client_t client = *((client_t*)data);

	int res;
	char buffer[BUFFER] = "";

	sprintf(buffer, "[Client %d] is connected.", client.id);
	broadcast(client.id, buffer);
	
	memset(buffer, 0, BUFFER);
	sprintf(buffer, "id: %" SCNd8, client.id);
	send(client.sock, buffer, BUFFER, false);
	
	while(client.sock > 0)
	{
		memset(buffer, 0, BUFFER);

		res = recv(client.sock, buffer, BUFFER, false);
		if(res <= 0)
		{
			break; /* Client is offline */
		}

		process_command(buffer, client.id);
	}

	printf("\n\n[Client %d] is disconnected.\n\n", client.id);
	memset(buffer, 0, BUFFER);
	sprintf(buffer, "[Client %d] is disconnected.", client.id);
	broadcast(client.id, buffer);
	close(client.sock);
	
	command_cursor();

	g_clients[client.id].status = FREE;
	return NULL;
}