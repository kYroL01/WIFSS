#include <server.h>

void broadcast(int sender, const char *msg)
{
	for(short int _i = 0; _i < MAX_CLIENTS; _i++)
	{
		if(_i != sender)
		{
			send(g_clients[_i].sock, msg, BUFFER, false);
		}
	}
}

void close_all_connections()
{
	printf("\n[WIFSS] Closing all connections...");

	char _buffer[BUFFER] = "";
	sprintf(_buffer, "%s", DISCONNECT);

	for(short int _i = 0; _i < MAX_CLIENTS; _i++)
	{
		if(g_clients[_i].status == TAKEN)
		{
			send(g_clients[_i].sock, _buffer, BUFFER, false);
			close(g_clients[_i].sock);
		}
		printf(".");
	}
	printf(" done.\n");
}

void closeServer()
{
	printf("[WIFSS] Server stopped.\n");
	for(short int _k = 0; _k < 60; _k++)
	{
		printf("=");
	}
	printf("\n\n");
}

inline void commandCursor()
{
	printf("|: ");
	fflush(stdout);
}

void process_command(const char *command, int sender_id)
{
	char _cpy[BUFFER]    = "";
	char _buffer[BUFFER] = "";
	bool _smthWritten    = true;

	if(str_beginwith(command, DOWNLOAD))
	{
		download(command, sender_id);
	}

	else if(str_beginwith(command, QUIT))
	{
		close(g_clients[sender_id].sock);
		return;
	}

	else if(str_beginwith(command, SENDP))
	{
		messagep(command, sender_id);
	}

	else if(str_beginwith(command, SEND))
	{
		message(command, sender_id);
	}

	else
	{
		printf("\n[WIFSS] Unknown command from [Client %d]: \"%s\".\n\n", sender_id, command);
	}

	commandCursor();
}

void* on_connection(void *data)
{
	struct client_t client = *((struct client_t*)data);

	int res;
	char _buffer[BUFFER] = "";

	sprintf(_buffer, "[Client %d] is connecting...", client.id);
	broadcast(client.id, _buffer);
	
	while(client.sock > 0)
	{
		memset(_buffer, 0, BUFFER);

		res = recv(client.sock, _buffer, BUFFER, false);
		if(res <= 0)
		{
			break; //Client is offline
		}

		process_command(_buffer, client.id);
	}

	printf("\n\n[Client %d] Deconnection...\n\n", client.id);
	sprintf(_buffer, "[Client %d] is deconnecting...", client.id);
	broadcast(client.id, _buffer);
	close(client.sock);
	
	commandCursor();

	g_clients[client.id].status = FREE;
	return NULL;
}

void startServer(void)
{
	int res = 0, i;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int listen_socket, sock;
	unsigned int current_id = 0, asize, count = 0;

	listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	server.sin_family 		= AF_INET;
	server.sin_addr.s_addr 	= INADDR_ANY;

	printf("\n\033[32m[WIFSS] Starting Server...\033[0m");

	do
	{
		printf("\n\n-> Listening Port: ");
		scanf("%d", &i);
		getchar();

		server.sin_port = htons(i);

		res = bind(listen_socket, (struct sockaddr*)&server, sizeof(server));
		if(res < 0)
		{
			char _buff[BUFFER];
			printf("\n\n\033[31m[WIFSS] Error during creation of listening socket [bind (%d)].\033[0m\n", res);
			printf("\nWould you like to retry on another port ? (Yes / No)\n\n");

			do
			{
				printf("|: ");
				promptKeyboard(_buff);

				if(!strcmp(_buff, "no") || !strcmp(_buff, "n"))
				{
					printf("\n\n");
					return;
				}

			} while(strcmp(_buff, "yes") && strcmp(_buff, "y"));
		}

	} while(res < 0);

	listen(listen_socket, MAX_CLIENTS);

	printf("\n\033[32m[WIFSS] Initialisation of Clients list [%d client slots available]...\n", MAX_CLIENTS);
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		g_clients[i].status = FREE;
	}
	printf("[WIFSS] Server opened, waiting for Clients on port %hu...\033[0m\n\n", (unsigned short int)ntohs(server.sin_port));

	pthread_create(&command_thread, NULL, &command_handler, (void*)&listen_socket);

	asize = sizeof(struct sockaddr_in);
	while(sock = accept(listen_socket, (struct sockaddr*)&client, &asize))
	{
		if(count + 1 >= MAX_CLIENTS)
		{
			char _buffer[BUFFER] = "";
			printf("\n\n[WIFSS] Maximum capacity reached, can't accept a new client yet... (%s:%hu)\n", inet_ntoa(client.sin_addr), (unsigned short int)ntohs(client.sin_port));
			sprintf(_buffer, "%s", "Maximum capacity reached, no more slot available for you yet.");
			send(sock, _buffer, BUFFER, false);
			close(sock);
			commandCursor();
			continue;
		}

		struct client_t new_client;

		for(i = 0; i < MAX_CLIENTS; i++)
		{
			if(g_clients[i].status == FREE)
			{
				current_id = i;
				g_clients[i].status = TAKEN;
				break;
			}
		}
		
		printf("\n\n[WIFSS] Connection received %s:%hu -> ID given: %u.\n", inet_ntoa(client.sin_addr), (unsigned short int)ntohs(client.sin_port), current_id);
		
		new_client.id 		= current_id;
		new_client.addr 	= client;
		new_client.sock 	= sock;
		new_client.status	= TAKEN;

		g_clients[current_id] = new_client;

		res = pthread_create(&threads[current_id], NULL, &on_connection, (void*)&new_client);
		if(res != 0)
		{
			printf("\033[31m[WIFSS] Error during Thread creation %d: Error (%d).\033[0m\n", current_id, res);
			broadcast(SID, "Server fatal error, stopping now.\n");
			close_all_connections();
			return;
		}

		/* Let's count clients online */
		count = 0;
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			if(g_clients[i].status == TAKEN)
			{
				count++;
			}
		}

		printf("[WIFSS] There is %d client(s) connected.\n\n", count);

		commandCursor();
	}

	close_all_connections();

	if(close(listen_socket) == -1)
	{
		printf("\n\033[35m[WIFSS] Socket of server couldn't be successfully closed.\033[0m\n");
	}

	closeServer();
}

int main(void)
{
	startServer();

	return 0;
}
