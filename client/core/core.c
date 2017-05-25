#include "client.h"


bool start_client(void)
{
	clear_console();

	printf("\n\033[32m[WIFSS] Starting Client...\033[0m\n\n");

	init_global_variables();

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

	struct addrinfo *servinfo;

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	int16_t status = getaddrinfo(address, args[0], &hints, &servinfo);
	if(status != 0)
	{
	    fprintf(stderr, "\n\033[31m[WIFSS] An error occurred while trying to get some information about your host: %s.\n\033[0m\n\n", gai_strerror(status));
	    exit(1);
	}

	int16_t sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1)
	{
		printf("\n\033[31m[WIFSS] Error while creating an endpoint for communication with server: %s.\033[0m\n\n", strerror(errno));
		return false;
	}

	char buffer[BUFFER];
	int16_t result = -1;
	do
	{
		for(struct addrinfo *tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
		{
			result = connect(sock, tmp->ai_addr, sizeof(*tmp->ai_addr));
			if(result != -1)
			{
				printf("\n\033[32m[WIFSS] Connected to ");
				printEndpoint(tmp);
				printf(".\033[0m\n");

				strncpy(buffer, "", BUFFER);
				recv(sock, buffer, BUFFER, false);
				uint8_t temp_id = getSecondArgsGroupAsInteger(buffer);
				printf("\n\033[32m[WIFSS] Your id on the server is %d.\033[0m\n\n", temp_id);

				/* We save the ID of the client for the future */
				g_core_variables.client_id = temp_id;

				break;
			}

			else
			{
				printf("\n\033[31m[WIFSS] Error while connecting to ");
				printEndpoint(tmp);
				printf(": %s.\033[0m\n", strerror(errno));
			}
		}

		if(result != -1)
		{
			// Yeah... We've to `break` the loop a second time 
			break;
		}

		printf("\nWould you like to retry now ? (YES / no)\n\n");
		if(!prompt_yes_no(buffer, args, &nbArgs))
		{
			printf("\n");
			return false;
		}

	} while(true);

	// We free here the previous linked list
	freeaddrinfo(servinfo);

	/* We save the server's port, socket and address values for the future */
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
	
	printf("[WIFSS] Client is shutting down for now.\n");

	for(uint8_t i = 0; i < 64; i++)
	{
		printf("=");
	}

	printf("\n\n");
}


void init_global_variables(void)
{
	g_core_variables.server_sock = -1;
	g_core_variables.server_port = -1;
	g_core_variables.client_id   = -1;
}
