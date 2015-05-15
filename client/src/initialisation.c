#include <client.h>

bool init(struct sockaddr_in *SERVER, int *sock)
{
	char _buff[BUFFER] = {0};
	short int _port;

	printf("\n\033[32mStarting client...\033[0m\n\n");
	sleep(1);

	do
	{
		printf("Server IP: ");
		gets(_buff);
		
	} while(!strcmp(_buff, "\0"));

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

	SERVER->sin_family      = AF_INET;
	SERVER->sin_port        = htons(_port);
	SERVER->sin_addr.s_addr = inet_addr(_buff);

	*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int _result;
	do
	{
		_result = connect(*sock, (struct sockaddr*)SERVER, sizeof(*SERVER));

		if(_result < 0)
		{
			printf("\n\n\033[31mError while connecting to %s:%hd.\nDo you want retry ? (Yes / No)\033[0m\n\n:| ", inet_ntoa(SERVER->sin_addr), _port);
			scanf("%s", _buff);
			getchar();

			str_lowerCase(_buff);

			if(!strcmp(_buff, "no") || !strcmp(_buff, "n"))
			{
				return false;
			}
		}

		else
		{
			printf("\nConnected to %s:%hd.\n\n", inet_ntoa(SERVER->sin_addr), ntohs(SERVER->sin_port));
		}

	} while(_result < 0 && (!strcmp(_buff, "yes") || !strcmp(_buff, "y")));

	_tunnelOpened_ = false;

	return true;
}


void disconnect(int sock)
{
	sleep(1);

	if(close(sock) == -1)
	{
		printf("\n\033[35mSocket couldn't be successfully closed.\033[0m\n");
	}

	printf("\n\n\033[35mSocket successfully closed.\033[0m\n\n");
	sleep(1);
	printf("WIFFS Client is shutting down for now !\n\n\n");
	sleep(1);
}
