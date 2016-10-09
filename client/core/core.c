#include "client.h"


bool start_client(struct sockaddr_in *server)
{
	/* Global core variables initialization */
	strcpy(g_core_variables.server_addr, "");
	g_core_variables.server_sock = -1;
	g_core_variables.server_port = -1;
	g_core_variables.client_id   = -1;

	system("clear");
	printf("\n\033[32m[WIFSS] Starting Client...\033[0m\n");

	if(!set_work_dir())
	{
		return false;
	}

	if(!check_download_folder())
	{
		return false;
	}

	uint8_t c;
	char buff[BUFFER] = "";
	uint32_t port;

	printf("\n\n");

	do
	{
		printf("-> Server IP: ");
		prompt_keyboard(buff);

	} while(!strcmp(buff, "\0") || str_infinite_spaces(buff));

	if(!strcmp(buff, LOCALHOST) || !strcmp(buff, LOCAL))
	{
		strcpy(buff, "");
		sprintf(buff, "%s", ADDRLOCAL);
	}

	do
	{
		printf("-> Server Port: ");
		scanf("%" SCNu32, &port);

		while((c = getchar()) && c != '\n');

	} while(port < 1024 || port > 65535);

	server->sin_family      = AF_INET;
	server->sin_port        = htons(port);
	server->sin_addr.s_addr = inet_addr(buff);

	int16_t sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1)
	{
		perror("Création socket Client");
		return false;
	}

	int16_t result;
	do
	{
		result = connect(sock, (struct sockaddr*)server, sizeof(*server));

		if(result < 0)
		{
			printf("\n\033[31m[WIFSS] Error while connecting to %s:%" SCNu32 ".\033[0m\n", inet_ntoa(server->sin_addr), port);
			printf("\nWould you like to retry now ? (Yes / No)\n\n");

			do
			{
				printf("|: ");
				prompt_keyboard(buff);

				if(!strcmp(buff, "no") || !strcmp(buff, "n"))
				{
					return false;
				}

			} while(strcmp(buff, "yes") && strcmp(buff, "y"));
		}

		else
		{
			printf("\n\033[32m[WIFSS] Connected to %s:%hd.\033[0m\n", inet_ntoa(server->sin_addr), ntohs(server->sin_port));
			
			uint8_t clientId = -1;
			strcpy(buff, "");
			recv(sock, buff, BUFFER, false);
			sscanf(buff, "id: %" SCNu8, &clientId);
			printf("\n\033[32m[WIFSS] Your id on the server is %" SCNu8 ".\033[0m\n\n", clientId);

			/* We save the ID of the client for the future */
			g_core_variables.client_id = clientId;
		}

	} while(result < 0);

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
			printf("\n\033[35m[WIFSS] Socket couldn't be successfully closed.\033[0m\n");
		}
		else
		{
			printf("\n\n[WIFSS] Socket successfully closed.\n");
		}
	}
	
	printf("[WIFSS] Client is shutting down for now.\n");
	for(uint8_t i = 0; i < 60; i++)
	{
		printf("=");
	}
	printf("\n\n");
}
