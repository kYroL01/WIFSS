#include "client.h"


bool start_client(struct sockaddr_in *server)
{
	clear_console();

	printf("\n\033[32m[WIFSS] Starting Client...\033[0m\n");

	init_global_variables();

	if(!set_work_dir() || !check_download_folder())
	{
		return false;
	}

	char address[BUFFER];
	char *args[BUFFER];
	uint16_t nbArgs;

	do
	{
		printf("-> Server IP: ");
		prompt_keyboard(address);
		free_args(args, &nbArgs);
		parse_command(address, args, &nbArgs);

	} while(nbArgs != 1);

	if(!strcmp(args[0], LOCALHOST))
	{
		strncpy(address, "", BUFFER);
		snprintf(address, BUFFER, "%s", ADDRLOCAL);
	}


	int32_t port;
	char portBuffer[BUFFER];
	do
	{
		printf("-> Server Port: ");
		prompt_keyboard(portBuffer);
		free_args(args, &nbArgs);
		parse_command(portBuffer, args, &nbArgs);

		if(nbArgs == 1)
		{
			port = strtoul(args[0], NULL, 10);
		}

		else
		{
			port = -1;
		}

	} while(port < 0 || port > 65535);

	server->sin_family      = AF_INET;
	server->sin_port        = htons(port);
	server->sin_addr.s_addr = inet_addr(address);

	int16_t sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1)
	{
		printf("\n\033[31m[WIFSS] Error while creating an endpoint for communication with server: %s.\033[0m\n\n", strerror(errno));
		return false;
	}

	char buffer[BUFFER];
	int16_t result;
	do
	{
		result = connect(sock, (struct sockaddr*)server, sizeof(*server));

		if(result == -1)
		{
			printf("\n\033[31m[WIFSS] Error while connecting to \'%s:%" SCNu32 "\': %s.\033[0m\n\n", inet_ntoa(server->sin_addr), server->sin_port, strerror(errno));
			printf("\nWould you like to retry now ? (YES / no)\n\n");

			if(!prompt_yes_no(buffer, args, &nbArgs))
			{
				printf("\n");
				return false;
			}
		}

		else
		{
			printf("\n\033[32m[WIFSS] Connected to \'%s:%hd\'.\033[0m\n", inet_ntoa(server->sin_addr), ntohs(server->sin_port));
			
			strncpy(buffer, "", BUFFER);

			recv(sock, buffer, BUFFER, false);

			uint8_t temp_id = getSecondArgsGroupAsInteger(buffer);
			printf("\n\033[32m[WIFSS] Your id on the server is %" SCNu8 ".\033[0m\n\n", temp_id);

			/* We save the ID of the client for the future */
			g_core_variables.client_id = temp_id;
		}

	} while(result == -1);

	/* We save the server's port, socket and address values for the future */
	strcpy(g_core_variables.server_addr, inet_ntoa(server->sin_addr));
	g_core_variables.server_sock = sock;
	g_core_variables.server_port = port;

	return true;
}


void stop_client(void)
{
	if(g_core_variables.server_sock >= 0)
	{
		if(close(g_core_variables.server_sock) == -1)
		{
			printf("\n\033[35m[WIFSS] Socket couldn't be successfully closed.\033[0m\n\n");
		}
		else
		{
			printf("\n\n[WIFSS] Socket successfully closed.\n\n");
		}
	}

	free(g_core_variables.working_dir);
	g_core_variables.working_dir = NULL;
	
	printf("[WIFSS] Client is shutting down for now.\n");

	for(uint8_t i = 0; i < 64; i++)
	{
		printf("=");
	}

	printf("\n\n");
}


void init_global_variables(void)
{
	strncpy(g_core_variables.server_addr, "", 16);
	g_core_variables.server_sock = -1;
	g_core_variables.server_port = -1;
	g_core_variables.client_id   = -1;
	g_core_variables.working_dir = NULL;
}
