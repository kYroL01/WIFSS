#include "server.h"


void start_server(void)
{
	system("clear");

	char c;
	int res = 0;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int listen_socket, sock;
	unsigned int currentid = 0, asize, count = 0;

	listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	server.sin_family      = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;

	printf("\n\033[32m[WIFSS] Starting Server...\033[0m\n");

	uint32_t port;
	do
	{
		do
		{
			printf("\n-> Listening Port: ");
			scanf("%" SCNu32, &port);

			while((c = getchar()) && c != '\n');

		} while((port < 1024 && geteuid() != 0) || port > 65535);

		server.sin_port = htons(port);
		g_server_port = port; /* We save this value to the global variable */

		res = bind(listen_socket, (struct sockaddr*)&server, sizeof(server));
		if(res < 0)
		{
			char _buff[BUFFER];
			printf("\n\n\033[31m[WIFSS] Error during creation of the listening socket [bind (%d)].\033[0m\n", res);
			printf("\nWould you like to retry on another port ? (Yes / No)\n\n");

			do
			{
				printf("|: ");
				prompt_keyboard(_buff);

				if(!strcmp(_buff, "no") || !strcmp(_buff, "n"))
				{
					printf("\n\n");
					return;
				}

			} while(strcmp(_buff, "yes") && strcmp(_buff, "y"));
		}

	} while(res < 0);

	listen(listen_socket, MAX_CLIENTS);

	printf("\n\033[32m[WIFSS] Initialization of clients list [%d client slots available]...\n", MAX_CLIENTS);
	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		g_clients[i].status = FREE;
	}
	printf("[WIFSS] Server opened, waiting for clients on port %hu...\033[0m\n\n", (unsigned short int)ntohs(server.sin_port));

	pthread_create(&g_command_thread, NULL, &command_handler, (void*)&listen_socket);

	asize = sizeof(struct sockaddr_in);
	while((sock = accept(listen_socket, (struct sockaddr*)&client, &asize)))
	{
		if(count + 1 >= MAX_CLIENTS)
		{
			char _buffer[BUFFER] = "";
			printf("\n\n[WIFSS] Maximum capacity reached, can't accept a new client yet... (%s:%hu)\n", inet_ntoa(client.sin_addr), (unsigned short int)ntohs(client.sin_port));
			sprintf(_buffer, "%s", "Maximum capacity reached, no more slot available for you yet.");
			send(sock, _buffer, BUFFER, false);
			close(sock);
			command_cursor();
			continue;
		}

		client_t new_client;

		for(uint8_t i = 0; i < MAX_CLIENTS; i++)
		{
			if(g_clients[i].status == FREE)
			{
				currentid = i;
				g_clients[i].status = TAKEN;
				break;
			}
		}
		
		printf("\n\n[WIFSS] Connection received %s:%hu -> ID given: %u.\n", inet_ntoa(client.sin_addr), (unsigned short int)ntohs(client.sin_port), currentid);
		
		new_client.id     = currentid;
		new_client.addr   = client;
		new_client.sock   = sock;
		new_client.status = TAKEN;

		g_clients[currentid] = new_client;

		res = pthread_create(&g_threads[currentid], NULL, &on_connection, (void*)&new_client);
		if(res != 0)
		{
			printf("\033[31m[WIFSS] Error during Thread creation %d: Error (%d).\033[0m\n", currentid, res);
			broadcast(SID, "Server fatal error, stopping now.\n");
			close_all_connections();
			return;
		}

		/* Let's count the number of clients online */
		count = 0;
		for(uint8_t i = 0; i < MAX_CLIENTS; i++)
		{
			if(g_clients[i].status == TAKEN)
			{
				count++;
			}
		}

		printf("[WIFSS] There is %d client(s) connected.\n\n", count);

		command_cursor();
	}

	close_server(listen_socket);
}


void close_server(int listen_socket)
{
	close_all_connections();

	if(close(listen_socket) == -1)
	{
		printf("\n\033[35m[WIFSS] Socket of server couldn't be successfully closed.\033[0m\n");
	}
	else
	{
		printf("\n\033[32m[WIFSS] Socket of server successfully closed.\033[0m\n");
	}

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		pthread_cancel(g_threads[i]);
	}

	printf("[WIFSS] Server stopped.\n");
	for(uint8_t i = 0; i < 60; i++)
	{
		printf("=");
	}
	printf("\n\n");
}


void broadcast(int sender, const char *msg)
{
	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		if(i != sender)
		{
			send(g_clients[i].sock, msg, BUFFER, false);
		}
	}
}


void disconnect(const char *buffer)
{
	int8_t idTemp = -2;
	sscanf(buffer, "disconnect %" SCNd8, &idTemp);
	if(idTemp == -1)
	{
		close_all_connections(); 
		printf("\n");
	}
	else if(idTemp >= 0 && idTemp < MAX_CLIENTS)
	{
		if(g_clients[idTemp].status == TAKEN)
		{
			send(g_clients[idTemp].sock, DISCONNECT, BUFFER, false);
			close(g_clients[idTemp].sock);
		}
		else
		{
			printf("\n[WIFSS] This client is offline yet. Can't disconnect him.\n\n");
		}
	}
	else
	{
		printf("\n[WIFSS] This client identifier is invalid.\n\n");
	}
}


void close_all_connections(void)
{
	printf("\n[WIFSS] Closing all connections...");

	char buffer[BUFFER] = DISCONNECT;

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		if(g_clients[i].status == TAKEN)
		{
			send(g_clients[i].sock, buffer, BUFFER, false);
		}

		printf(".");
	}

	printf(" done.\n");
}
