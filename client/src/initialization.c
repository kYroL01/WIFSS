#include <client.h>

_Bool start(struct sockaddr_in *server, int *sock)
{
	char c;
	char buff[BUFFER] = "";
	unsigned short int port;

	printf("\n\n");

	do
	{
		printf("-> Server IP: ");
		promptKeyboard(buff);

	} while(!strcmp(buff, "\0") || str_infiniteSpaces(buff));

	if(!strcmp(buff, LOCALHOST) || !strcmp(buff, LOCAL))
	{
		memset(buff, 0, BUFFER);
		sprintf(buff, "%s", ADDRLOCAL);
	}

	do
	{
		printf("-> Server Port: ");
		scanf("%hu", &port);

		while((c = getchar()) && c != '\n');

	} while(port < 1024 || port > 65535);

	server->sin_family      = AF_INET;
	server->sin_port        = htons(port);
	server->sin_addr.s_addr = inet_addr(buff);

	*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int result;
	do
	{
		result = connect(*sock, (struct sockaddr*)server, sizeof(*server));

		if(result < 0)
		{
			printf("\n\033[31m[WIFSS] Error while connecting to %s:%hd.\033[0m\n", inet_ntoa(server->sin_addr), port);
			printf("\nWould you like to retry now ? (Yes / No)\n\n");

			do
			{
				printf("|: ");
				promptKeyboard(buff);

				if(!strcmp(buff, "no") || !strcmp(buff, "n"))
				{
					return false;
				}

			} while(strcmp(buff, "yes") && strcmp(buff, "y"));
		}

		else
		{
			printf("\n\033[32m[WIFSS] Connected to %s:%hd.\033[0m\n", inet_ntoa(server->sin_addr), ntohs(server->sin_port));
			
			short int clientId = -1;
			memset(buff, 0, BUFFER);
			recv(*sock, buff, BUFFER, false);
			sscanf(buff, "id: %hd", &clientId);
			printf("\n\033[32m[WIFSS] Your id on the server is %hd.\033[0m\n\n", clientId);
			CLIENT_ID = clientId;
		}

	} while(result < 0);

	/* We save the port and the address values for the future */
	SERVER_ADDR = inet_ntoa(server->sin_addr);
	SERVER_PORT = port;

	return true;
}


void stop(int sock)
{
	if(sock > 0)
	{
		if(close(sock) == -1)
		{
			printf("\n\033[35m[WIFSS] Socket couldn't be successfully closed.\033[0m\n");
		}
		else
		{
			printf("\n\n[WIFSS] Socket successfully closed.\n");
		}
	}
	
	printf("[WIFSS] Client is shutting down for now.\n");
	short int  i;
	for(i = 0; i < 60; i++)
	{
		printf("=");
	}
	printf("\n\n");
}
