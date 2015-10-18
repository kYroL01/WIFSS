#include <client.h>

_Bool init(struct sockaddr_in *server, int *sock)
{
	char _buff[BUFFER] = {0};
	short int _port;

	printf("\n\033[32m[WIFSS] Starting client...\033[0m\n\n");
	sleep(1);

	do
	{
		printf("Server IP: ");
		fgets(_buff, FGETSBUFF, stdin);
		str_removeSlashN(_buff);

	} while(!strcmp(_buff, "\0") || str_infiniteSpaces(_buff));

	str_lowerCase(_buff);


	if(!strcmp(_buff, LOCALHOST) || !strcmp(_buff, LOCAL))
	{
		memset(_buff, 0, BUFFER);
		sprintf(_buff, "%s", ADDRLOCAL);
	}


	do
	{
		printf("Port: ");
		scanf("%hd", &_port);
		getchar();

	} while(_port < 1024);

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

			do
			{
				printf("\nWould you like to retry now ? (Yes / No)\n|: ");
				scanf("%s", _buff);
				getchar();

				str_lowerCase(_buff);

				if(!strcmp(_buff, "no") || !strcmp(_buff, "n"))
				{
					return false;
				}

			} while(!strcmp(_buff, "yes") || !strcmp(_buff, "y"));
		}

		else
		{
			printf("\n[WIFSS] Connected to %s:%hd.\n\n", inet_ntoa(server->sin_addr), ntohs(server->sin_port));
		}

	} while(_result < 0);

	return true;
}


void disconnect(int sock)
{
	sleep(1);

	if(close(sock) == -1)
	{
		printf("\n\033[35m[WIFSS] Socket couldn't be successfully closed.\033[0m\n");
	}

	printf("\n\033[35m[WIFSS] Socket successfully closed.\033[0m\n");
	sleep(1);
	printf("[WIFSS] Client is shutting down for now !\n\n");
	for(short int _i = 0; _i < 60; _i++)
	{
		printf("=");
	}
	printf("\n\n");
	sleep(1);
}
