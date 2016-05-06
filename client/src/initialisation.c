#include <client.h>

_Bool start(struct sockaddr_in *server, int *sock)
{
	char _c;
	char _buff[BUFFER] = "";
	short int _port;

	printf("\n\n");

	do
	{
		printf("-> Server IP: ");
		promptKeyboard(_buff);

	} while(!strcmp(_buff, "\0") || str_infiniteSpaces(_buff));

	if(!strcmp(_buff, LOCALHOST) || !strcmp(_buff, LOCAL))
	{
		memset(_buff, 0, BUFFER);
		sprintf(_buff, "%s", ADDRLOCAL);
	}

	do
	{
		printf("-> Server Port: ");
		scanf("%hd", &_port);

		while((_c = getchar()) && _c != '\n');

	} while(_port < 1024 || _port > 65535);

	server->sin_family      = AF_INET;
	server->sin_port        = htons(_port);
	server->sin_addr.s_addr = inet_addr(_buff);

	*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int _result;
	do
	{
		_result = connect(*sock, (struct sockaddr*)server, sizeof(*server));

		if(_result < 0)
		{
			printf("\n\033[31m[WIFSS] Error while connecting to %s:%hd.\033[0m\n", inet_ntoa(server->sin_addr), _port);
			printf("\nWould you like to retry now ? (Yes / No)\n\n");

			do
			{
				printf("|: ");
				promptKeyboard(_buff);

				if(!strcmp(_buff, "no") || !strcmp(_buff, "n"))
				{
					return false;
				}

			} while(strcmp(_buff, "yes") && strcmp(_buff, "y"));
		}

		else
		{
			printf("\n\033[32m[WIFSS] Connected to %s:%hd.\033[0m\n", inet_ntoa(server->sin_addr), ntohs(server->sin_port));
			
			short int clientId = -1;
			memset(_buff, 0, BUFFER);
			recv(*sock, _buff, BUFFER, false);
			sscanf(_buff, "id: %hd", &clientId);
			printf("\n\033[32m[WIFSS] Your id on the server is %hd.\033[0m\n\n", clientId);
		}

	} while(_result < 0);

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
	short int  _i;
	for(_i = 0; _i < 60; _i++)
	{
		printf("=");
	}
	printf("\n\n");
}
