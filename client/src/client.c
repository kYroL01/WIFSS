#include <client.h>

void handle_command(const char *command, int sock, bool *connected)
{
	if(str_beginwith(command, QUIT))
	{
		printf("\n\nLet's close connection with Server...");
		*connected = false;
	}

	else if(str_beginwith(command, DOWNLOAD))
	{
		unsigned short _i, _j;
		char _path[BUFFER / 4];

		for(_i = strlen(DOWNLOAD) + 1, _j = 0; _i < strlen(command); _i++, _j++)
		{
			_path[_j] = command[_i];
		}

		if(!download(_path, sock))
		{
			printf("File couldn't be downloaded correctly.\n\n");
		}
		else
		{
			printf("File downloaded !\n\n");
		}
	}

	/*else if()
	{

	}*/

	else
	{
		printf("\nCommand unknown.\n");
	}
}

int main(void)
{
	int sock;
	bool connected;
	char buff[BUFFER];
	unsigned short int _i;
	struct sockaddr_in SERVER;

	pthread_t sthread;


	initialisation(&SERVER, &sock, &connected);


	pthread_create(&sthread, NULL, &scom, (void*)&sock);
	

	keepGoing = true;
	while(keepGoing && connected)
	{
		memset(buff, 0, BUFFER);
		printf("|: ");
		scanf("%s", buff);
		//send(sock, buff, BUFFER, false);

		//
		for(_i = 0; _i < strlen(buff) && buff[_i] != ' '; _i++) //On passe en minuscule le premier terme
		{
			buff[_i] = tolower(buff[_i]);
		}

		handle_command(buff, sock, &connected);
		//
	}



	disconnect(sock);

	return 1;
}


//gcc *.c -Wall -W -lpthread -I\../inc -o client_WIFFS && ./client_WIFFS