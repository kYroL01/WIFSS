#include <client.h>



int main(int argc, char const *argv[])
{
	short int port;
	struct sockaddr_in SERVER;

	initialisation(&SERVER, &port);
	
	
	bool keepGoing = true;
	while(keepGoing)
	{
		/*memset(buff, 0, BUFFER); //Dialogue basique

		printf("|: ");
		fgets(buff, BUFFER, stdin);

		send(sock, buff, BUFFER, 0);*/

		if(!strcmp(buff, END))
		{
			keepGoing = false;
			printf("\n\nLet's close connection with Server...");
		}
	}


	close(sock);
	printf("\n\033[35mConnection successfully closed.\033[35m\n\n");

	return 1;
}
