#include <client.h>



int main(void)
{
	int sock;
	bool connected;
	char buff[BUFFER] = {0};
	struct sockaddr_in SERVER;

	pthread_t sthread;


	initialisation(&SERVER, &sock, &connected);


	pthread_create(&sthread, NULL, &scom, (void*)&sock);
	

	keepGoing = true;
	while(keepGoing && connected)
	{
		printf("|: ");

		gets(buff);

		str_lowerCase(buff);

		handle_command(buff, sock, &connected);

		memset(buff, 0, BUFFER);
	}


	disconnect(sock);

	return 1;
}


//gcc *.c -Wall -W -lpthread -I\../inc -o client_WIFFS && ./client_WIFFS
