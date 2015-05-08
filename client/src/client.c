#include <client.h>

void handle_command(const char *command, int sock)
{
	/* si download, quit, etc ... */
}

int main(void)
{
	int sock;
	struct sockaddr_in SERVER;
	bool connected;
	char buff[BUFFER];

	pthread_t sthread;


	initialisation(&SERVER, &sock, &connected);

	keepGoing = true;

	pthread_create(&sthread, NULL, &scom, (void*) &sock);	
	while(keepGoing && connected)
	{
		memset(buff, 0, BUFFER);
		printf("|: ");
		scanf("%s", buff);
		// send(sock, buff, BUFFER, 0);
		
		handle_command(buff, sock);
		
		if(!strcmp(buff, END))
		{
			keepGoing = false;
			printf("\n\nLet's close connection with Server...");
		}
	}



	disconnection(sock);

	return 1;
}
