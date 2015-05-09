#include <client.h>



int main(void)
{
	int sock;
	bool connected;
	char buff[BUFFER];
	short int _i;
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
		for(_i = 0; _i < (short int)strlen(buff) && buff[_i] != ' '; _i++)
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
