#include "commands.h"


void* command_handler(void *foo)
{
	/* To get rid of '-Wextra' "Unused variable" warning */
	(void)foo;

	char buffer[BUFFER];
	char *args[BUFFER];
	uint16_t nbArgs;

	while(1)
	{
		command_cursor();
		prompt_keyboard(buffer);
		free_args(args, &nbArgs);
		parse_command(buffer, args, &nbArgs);

		// In order to avoid a segmentation fault if 'args' is empty...
		if(nbArgs == 0)
		{
			continue;
		}

		if(command_validation((const char* const*)args, nbArgs, EXIT, 1))
		{
			broadcast(SID, "[Server] is going to shutdown !");
			pthread_exit(NULL);
		}

		else if(str_beginwith(buffer, SEND))
		{
			char cpy[BUFFER]      = "";
			char buffTemp[BUFFER] = "";
			strncat(cpy, getSecondArgsGroup(buffer), BUFFER);
			snprintf(buffTemp, BUFFER, "[Server] says: \"%s\".", cpy);
			broadcast(SID, buffTemp);
		}

		else if(str_beginwith(buffer, WHISPER))
		{
			char cpy[BUFFER]      = "";
			char buffTemp[BUFFER] = "";

			int8_t idTemp = getSecondArgsGroupAsInteger(buffer);
			strncpy(cpy, getThirdArgsGroup(buffer), BUFFER);

			if(idTemp >= 0 && g_core_variables.clients[idTemp].status == TAKEN && idTemp < MAX_CLIENTS)
			{
				snprintf(buffTemp, BUFFER, "[Server] whispers to you: \"%s\".", cpy);
				send(g_core_variables.clients[idTemp].sock, buffTemp, BUFFER, false);
			}

			else
			{
				printf("\n[WIFSS] This client identifier is invalid.\n\n");
			}
		}

		else if(command_validation((const char* const*)args, nbArgs, DISCONNECT, ARGDISCONNECT))
		{
			disconnect(buffer);
		}

		else if(command_validation((const char* const*)args, nbArgs, CLEAR, 1))
		{
			clear_console();
		}

		else if(command_validation((const char* const*)args, nbArgs, WHO, 1))
		{
			who(SID);
		}

		else if(command_validation((const char* const*)args, nbArgs, HELP, 1))
		{
			static const char *helpMenu[32] =
			{
				"help",
				"who",
				"send <message>",
				"whisper <idClient> <message>",
				"disconnect <idClient> ['-1' for all]",
				"exit",
				"clear",
				"\n"
			};

			for(uint8_t j = 0; helpMenu[j] != NULL; j++)
			{
				printf("\t%s\n", helpMenu[j]);
			}
		}

		else
		{
			printf("\nUnknown command. Try \"help\" for further information.\n\n");
		}
	}

	close_server();
}


void process_command(const char *command, const uint8_t sender_id)
{
	if(str_beginwith(command, WHO))
	{
		who(sender_id);
	}

	else if(str_beginwith(command, SEND))
	{
		message(command, sender_id);
	}

	else if(str_beginwith(command, WHISPER))
	{
		whisper(command, sender_id);
	}

	else
	{
		printf("\n\n[WIFSS] Unknown command from [Client %d]: \"%s\".\n\n", sender_id, command);
	}

	command_cursor();
}


void* connections_handler(void *foo)
{
	/* In order to deal with '-Wextra' "Unused variable" warning */
	(void)foo;

	struct sockaddr_in client;
	uint32_t asize = sizeof(struct sockaddr_in);

	int8_t sock;
	uint8_t count = 0;
	while(1)
	{
		sock = accept(g_core_variables.server_sock, (struct sockaddr*)&client, &asize);
		if(sock == -1)
		{
			printf("\033[31m[WIFSS] Error during a tentative of accepting connection: %s.\033[0m\n\n", strerror(errno));
			continue;
		}

		if(count + 1 >= MAX_CLIENTS)
		{
			char buffer[BUFFER] = "";
			printf("\n\n[WIFSS] Maximum capacity reached, can't accept a new client yet... (%s:%" SCNu8 ")\n", inet_ntoa(client.sin_addr), (uint8_t)ntohs(client.sin_port));
			snprintf(buffer, BUFFER, "%s", "Maximum capacity reached, no more slot available for you yet.");
			send(sock, buffer, BUFFER, false);
			close(sock);
			command_cursor();
			continue;
		}

		uint8_t current_id = -1;
		for(uint8_t i = 0; i < MAX_CLIENTS; i++)
		{
			if(g_core_variables.clients[i].status == FREE)
			{
				current_id = i;
				g_core_variables.clients[i].status = TAKEN;
				break;
			}
		}
		
		printf("\n\n[WIFSS] Connection received \'%s:%" SCNu16 "\' -> ID given: %" SCNu8 ".\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), current_id);

		client_t new_client;
		new_client.id     = current_id;
		new_client.addr   = client;
		new_client.sock   = sock;
		new_client.status = TAKEN;

		g_core_variables.clients[current_id] = new_client;

		int16_t res = pthread_create(&g_core_variables.threads[current_id], NULL, &on_connection, (void*)&new_client);
		if(res != 0)
		{
			printf("\033[31m[WIFSS] Error during thread creation %d: Error (%d).\033[0m\n\n", current_id, res);
			broadcast(SID, "Server fatal error, stopping now.\n");
			close_all_connections();
			pthread_exit(NULL);
		}

		/* Let's count the number of clients online */
		count = 0;
		for(uint8_t i = 0; i < MAX_CLIENTS; i++)
		{
			if(g_core_variables.clients[i].status == TAKEN)
			{
				count++;
			}
		}

		printf("[WIFSS] There is %" SCNu8 " client(s) connected.\n\n", count);

		command_cursor();
	}
}


void* on_connection(void *data)
{
	client_t client = *((client_t*)data);

	char buffer[BUFFER] = "";

	snprintf(buffer, BUFFER, "[Client %" SCNd8 "] is connected.", client.id);
	broadcast(client.id, buffer);

	strncpy(buffer, "", BUFFER);
	snprintf(buffer, BUFFER, "id: %" SCNd8, client.id);
	send(client.sock, buffer, BUFFER, false);

	ssize_t res;
	while(client.sock > 0)
	{
		strncpy(buffer, "", BUFFER);

		res = recv(client.sock, buffer, BUFFER, false);
		if(res <= 0)
		{
			break; /* Client is offline */
		}

		process_command(buffer, client.id);
	}

	printf("\n\n[Client %" SCNd8 "] is disconnected.\n\n", client.id);

	strncpy(buffer, "", BUFFER);
	snprintf(buffer, BUFFER, "[Client %d] is disconnected.", client.id);
	broadcast(client.id, buffer);

	if(close(client.sock) == -1)
	{
		printf("\033[31m[WIFSS] Error while closing the socket of client %" SCNd8 ": %s.\033[0m\n\n", client.id, strerror(errno));
	}
	
	command_cursor();

	g_core_variables.clients[client.id].status = FREE;
	g_core_variables.clients[client.id].id     = -1;
	g_core_variables.clients[client.id].sock   = -1;

	return NULL;
}
