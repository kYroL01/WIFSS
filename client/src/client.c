#include <client.h>



int main(int argc, char const *argv[])
{
	int sock;
	struct sockaddr_in SERVER;
	bool connected;

	char buff[BUFFER];


	initialisation(&SERVER, &sock, &connected);


	bool keepGoing = true;
	while(keepGoing && connected)
	{
		/*memset(buff, 0, BUFFER); //Basic communication

		printf("|: ");
		fgets(buff, BUFFER, stdin);

		send(sock, buff, BUFFER, 0);*/

		if(!strcmp(buff, END))
		{
			keepGoing = false;
			printf("\n\nLet's close connection with Server...");
		}
	}



	disconnection(sock);

	return 1;
}
