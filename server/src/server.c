#include <server.h>

void broadcast(int sender, const char *msg)
{
	int i;
	char _buffer[BUFFER] = "";
	sprintf(_buffer, "%s", msg);
	
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(i != sender)
		{
			send(g_clients[i].sock, _buffer, BUFFER, false);
		}
	}
}

void close_all_connections()
{
	printf("\n[WIFSS] Closing all connections");
	char _buffer[BUFFER] = "";
	sprintf(_buffer, "%s", DISCONNECT);

	for(short int i = 0; i < MAX_CLIENTS; i++)
	{
		if(g_clients[i].status == TAKEN)
		{
			send(g_clients[i].sock, _buffer, BUFFER, false);
			close(g_clients[i].sock);
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
	printf("\n|: ");
	fflush(stdout);
}

int process_command(const char *command, int sender_id)
{
	int _fsize;
	char _cpy[BUFFER]    = "";
	char _buffer[BUFFER] = "";
	bool _smthWritten    = true;

	if(str_beginwith(command, DOWNLOAD))
	{
		int remote_id         = -1;
		char filename[BUFFER] = "";

		printf("\n\n[Client %d] Asking for upload !\n", sender_id);

		sscanf(command, "download %s %d", filename, &remote_id);
		printf("[Client %d] File: \"%s\" of %d\n", sender_id, filename, remote_id);

		if((remote_id > (MAX_CLIENTS - 1)) || (remote_id < 0) || (remote_id == sender_id))
		{
			send(g_clients[sender_id].sock, "Error: Client wanted is invalid...", BUFFER, false);
			return 0;
		}


		if(g_clients[remote_id].sock <= 0)
		{
			send(g_clients[sender_id].sock, "Error: Client asked is not connected...", BUFFER, false);
			return 0;
		}

		sprintf(_cpy, "upload %s", filename);
		send(g_clients[remote_id].sock, _cpy, BUFFER, false);

		recv(g_clients[remote_id].sock, _buffer, BUFFER, false);
		sscanf(_buffer, "size: %d", &_fsize);
		printf("[WIFSS] File size: %d.\n", _fsize);

		while(strcmp(_buffer, ENDT))
		{
			recv(g_clients[remote_id].sock, _buffer, BUFFER, false);
			printf("%s", _buffer);
		}
		return 0;
	}

	else if(str_beginwith(command, QUIT))
	{
		close(g_clients[sender_id].sock);
		return 1;
	}

	else if(str_beginwith(command, SENDP))
	{
		memset(_cpy, 0, BUFFER);
		memset(_buffer, 0, BUFFER);
		short int _idTemp = -1;
		sscanf(command, "sendp %hd %[^\n]", &_idTemp, _cpy);
		if(g_clients[_idTemp].status == TAKEN)
		{
			sprintf(_buffer, "[Client %d] whispers: \"%s\".", sender_id, _cpy);
			send(g_clients[_idTemp].sock, _buffer, BUFFER, false);
			printf("\n\n[Client %d] whispers to [Client %d]: \"%s\".\n", sender_id, _idTemp, _cpy);
		}
		else
		{
			send(g_clients[sender_id].sock, "ERROR: Client not connected.", BUFFER, false);
			printf("\n\n[Client %d] tried to whisper to [Client %d]: \"%s\", but he is not connected.\n", sender_id, _idTemp, _cpy);
		}
	}

	else if(str_beginwith(command, SEND))
	{
		memset(_cpy, 0, BUFFER);
		memset(_buffer, 0, BUFFER);
		sscanf(command, "send %[^\n]", _buffer);
		sprintf(_cpy, "[Client %d] : \"%s\".", sender_id, _buffer);
		broadcast(sender_id, _cpy);
		printf("\n\n[Client %d] says: \"%s\".\n", sender_id, _buffer);
	}

	else
	{
		printf("\n\n[WIFSS] Unknown command from [Client %d]: \"%s\".\n", sender_id, command);
	}

	commandCursor();

	return 1;
}

void* on_connection(void *data)
{
	int res;
	char _buffer[BUFFER] = "";
	struct client_t client = *((struct client_t*)data);

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

	printf("\n\n[Client %d] Deconnection...\n", client.id);
	sprintf(_buffer, "[Client %d] is deconnecting...", client.id);
	broadcast(client.id, _buffer);
	close(client.sock);
	
	commandCursor();

	g_clients[client.id].status = FREE;
	return NULL;
}

int startServer(void)
{
	int res = 0, i;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int listen_socket, sock;
	unsigned int current_id = 0, asize;

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
			printf("\nWould you like to retry now ? (Yes / No)\n\n");

			do
			{
				printf("|: ");
				promptKeyboard(_buff);

				if(!strcmp(_buff, "no") || !strcmp(_buff, "n"))
				{
					return res;
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
	printf("[WIFSS] Server opened, waiting for Clients...\033[0m\n\n");

	pthread_create(&command_thread, NULL, &command_handler, (void*)&listen_socket);

	asize = sizeof(struct sockaddr_in);
	while((sock = accept(listen_socket, (struct sockaddr*)&client, &asize)) && (count < MAX_CLIENTS))
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
			return res;
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

		printf("[WIFSS] There is %d client(s) connected.\n", count);

		commandCursor();
	}

	close_all_connections();

	if(close(listen_socket) == -1)
	{
		printf("\n\033[35m[WIFSS] Socket of server couldn't be successfully closed.\033[0m\n");
	}

	closeServer();

	return 0;
}

int main(void)
{
	startServer();

	return 0;
}

