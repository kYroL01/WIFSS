#include "server.h"


void start_server(void)
{
	clear_console();

	printf("\n\033[32m[WIFSS] Starting Server...\033[0m\n\n");

	init_global_variables();

	int8_t listen_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(listen_socket == -1)
	{
		printf("\n\n\033[31m[WIFSS] Error during creation of an endpoint for listening socket: %s.\033[0m\n\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server;
	server.sin_family      = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;

	char buffer[BUFFER];
	char *args[BUFFER];
	uint16_t nbArgs;

	int8_t res;
	uint32_t port;
	do
	{
		do
		{
			printf("\n-> Listening Port: ");
			prompt_keyboard(buffer);
			free_args(args, &nbArgs);
			parse_command(buffer, args, &nbArgs);

			if(nbArgs == 1)
			{
				port = strtoul(args[0], NULL, 10);
			}

			else
			{
				port = -1;
			}

		} while((port < 1024 && geteuid() != 0) || port > 65535);

		server.sin_port = htons(port);

		/* Let's save these values to the global variables structure */
		g_core_variables.server_port = port;
		g_core_variables.server_sock = listen_socket;

		res = bind(listen_socket, (struct sockaddr*)&server, sizeof(server));
		if(res == -1)
		{
			printf("\n\n\033[31m[WIFSS] Error during creation of the listening socket: %s.\033[0m\n\n", strerror(errno));
			printf("\nWould you like to retry on another port ? (YES / no)\n\n");

			if(!prompt_yes_no(buffer, args, &nbArgs))
			{
				close_server();
				exit(EXIT_FAILURE);
			}
		}

	} while(res == -1);

	if(listen(listen_socket, MAX_CLIENTS) == -1)
	{
		printf("\n\n\033[31m[WIFSS] Error during listening on the socket: %s.\033[0m\n\n", strerror(errno));
		close_server();
		exit(EXIT_FAILURE);
	}

	printf("\n\n[WIFSS] Server opened, waiting for clients on port %" SCNu16 "...\033[0m\n\n", ntohs(server.sin_port));
}


void close_server(void)
{
	close_all_connections();

	if(g_core_variables.server_sock >= 0)
	{
		if(close(g_core_variables.server_sock) == -1)
		{
			printf("\n\033[35m[WIFSS] Socket of server couldn't be successfully closed.\033[0m\n\n");
		}
		else
		{
			printf("\n\033[32m[WIFSS] Socket of server successfully closed.\033[0m\n\n");
		}
	}

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		pthread_cancel(g_core_variables.threads[i]);
	}

	printf("[WIFSS] Server is shutting down for now.\n");

	for(uint8_t i = 0; i < 64; i++)
	{
		printf("=");
	}

	printf("\n\n");
}


void init_global_variables(void)
{
	printf("\n\033[32m[WIFSS] Initialization of clients list [%d client slots would be available]...\033[0m\n", MAX_CLIENTS);

	g_core_variables.server_port = -1;
	g_core_variables.server_sock = -1;

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		g_core_variables.clients[i].id = -1;
		g_core_variables.clients[i].sock = -1;
		g_core_variables.clients[i].status = FREE;
	}
}
