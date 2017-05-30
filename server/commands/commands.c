#include "commands.h"


void* command_handler(void *foo)
{
	/* To get rid of '-Wextra' "Unused variable" warning */
	(void)foo;

	char buffer[BUFFER];
	char *args[BUFFER];
	int16_t nbArgs = -1;

	while(true)
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

		else if(str_beginwith(buffer, SEND) && nbArgs >= 3)
		{
			char cpy[BUFFER]      = "";
			char buffTemp[BUFFER] = "";
			strncat(cpy, get_second_args_group(buffer), BUFFER);
			snprintf(buffTemp, BUFFER, "[Server] says: \"%s\".", cpy);
			broadcast(SID, buffTemp);
		}

		else if(str_beginwith(buffer, WHISPER) && nbArgs >= 3)
		{
			char cpy[BUFFER]      = "";
			char buffTemp[BUFFER] = "";

			int16_t idTemp = get_second_args_group_as_integer(buffer);
			strncpy(cpy, get_third_arg_group(buffer), BUFFER);

			if(idTemp >= 0 && core_variables.clients[idTemp].status == TAKEN && idTemp < MAX_CLIENTS)
			{
				snprintf(buffTemp, BUFFER, "[Server] whispers to you: \"%s\".", cpy);
				SSL_write(core_variables.clients[idTemp].ssl, buffTemp, BUFFER);
			}

			else
			{
				printf("\n\033[35m[WIFSS] This client identifier is invalid.\033[0m\n\n");
			}
		}

		else if(command_validation((const char* const*)args, nbArgs, DISCONNECT, 2))
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
				""
			};

			for(uint8_t j = 0; helpMenu[j] != NULL; j++)
			{
				printf("\t%s\n", helpMenu[j]);
			}
		}

		else
		{
			printf("\n\033[35mUnknown command. Try \"help\" for further information.\033[0m\n\n");
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
		printf("\n\n\033[35m[WIFSS] Unknown command from [Client %d]: \"%s\".\033[0m\n\n", sender_id, command);
	}

	command_cursor();
}


void* connections_handler(void *foo)
{
	/* In order to deal with '-Wextra' "Unused variable" warning */
	(void)foo;

	struct sockaddr_in6 client;
	uint32_t asize = sizeof(client);

	int8_t sock;
	uint8_t count = 0;
	while(true)
	{
		sock = accept(core_variables.server_sock, (struct sockaddr*)&client, &asize);
		if(sock == -1)
		{
			printf("\033[31m[WIFSS] Error during a connection acceptation: %s.\033[0m\n\n", strerror(errno));
			continue;
		}

		char host[1024]  = "";
		char service[32] = "";

		if(getnameinfo((struct sockaddr*)&client, sizeof(client), host, sizeof(host), service, sizeof(service), 0) != 0)
		{
			printf("\033[31m[WIFSS] An error occurred while retrieving some information about : %s.\033[0m\n\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if(count >= MAX_CLIENTS)
		{
			printf("\n\n\033[35m[WIFSS] Maximum capacity reached, can\'t accept a new client yet... (%s [%s])\033[0m\n\n", host, service);

			const char *const local = "\n\033[35m[Server] Maximum capacity reached, please try again later !\033[0m\n";
			send(sock, local, strlen(local), false);

			close(sock);
			command_cursor();
			continue;
		}

		else
		{
			// Whereas the connection has been accepted, just send a packet in order to synchronize the `select` call...
			send(sock, CLIENT_SERVER_SYNC, strlen(CLIENT_SERVER_SYNC), false);
		}

		// Enables SSL !
		SSL *ssl = SSL_new(core_variables.ctx);
		SSL_set_fd(ssl, sock);

		if(SSL_accept(ssl) <= 0)
		{
			ERR_print_errors_fp(stderr);
			printf("\n\n\033[35m[WIFSS] Can\'t accept the SSL connection with the client \'%s [%s]\'\033[0m\n\n", host, service);
			close(sock);
			command_cursor();
			continue;
		}

		uint8_t current_id = -1;
		for(uint8_t i = 0; i < MAX_CLIENTS; i++)
		{
			if(core_variables.clients[i].status == FREE)
			{
				current_id = i;
				core_variables.clients[i].status = TAKEN;
				break;
			}
		}

		printf("\n\n\033[35m[WIFSS] Connection received \'%s [%s]\' -> ID given: %d.\033[0m\n", host, service, current_id);

		Client new_client;
		new_client.id     = current_id;
		new_client.addr   = client;
		new_client.sock   = sock;
		new_client.status = TAKEN;
		new_client.ssl    = ssl;

		core_variables.clients[current_id] = new_client;

		int16_t res = pthread_create(&core_variables.threads[current_id], NULL, &on_connection, (void*)&new_client);
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
			if(core_variables.clients[i].status == TAKEN)
			{
				count++;
			}
		}

		printf("\033[35m[WIFSS] There is %d client%s connected.\033[0m\n\n", count, (count > 1 ? "s" : ""));

		command_cursor();
	}
}


void* on_connection(void *data)
{
	Client client = *((Client*)data);

	char buffer[BUFFER] = "";

	snprintf(buffer, BUFFER, "[Client %d] is connected.", client.id);
	broadcast(client.id, buffer);

	strncpy(buffer, "", BUFFER);
	snprintf(buffer, BUFFER, "id: %d", client.id);
	SSL_write(client.ssl, buffer, BUFFER);

	do
	{
		strncpy(buffer, "", BUFFER);

		if(SSL_read(client.ssl, buffer, BUFFER) <= 0)
		{
			fprintf(stderr, "\n\n\033[35m[WIFSS] [Client %d]\'s socket has been closed. He\'ll be disconnected from now.\033[0m\n\n", client.id);
			break; /* Client is offline */
		}

		process_command(buffer, client.id);

	} while(true);

	strncpy(buffer, "", BUFFER);
	snprintf(buffer, BUFFER, "[Client %d] is disconnected.", client.id);
	broadcast(client.id, buffer);

	SSL_free(client.ssl);

	if(close(client.sock) == -1)
	{
		printf("\033[35m[WIFSS] Warning: couldn\'t close the [Client %d]\'s socket (already closed ?): %s.\033[0m\n\n", client.id, strerror(errno));
	}

	command_cursor();

	core_variables.clients[client.id].status = FREE;
	core_variables.clients[client.id].id     = -1;
	core_variables.clients[client.id].sock   = -1;
	core_variables.clients[client.id].ssl    = NULL;

	return NULL;
}
