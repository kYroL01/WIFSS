#include <client.h>



int main(void)
{
	int sock;
	bool keepGoing;
	struct sockaddr_in SERVER;

	pthread_t sthread;


	initialisation(&SERVER, &sock, &keepGoing);


	pthread_create(&sthread, NULL, &scom, (void*)&sock);


	while(keepGoing && _sComOn_)
	{
		communication(sock, &keepGoing, false);
	}


	disconnect(sock);

	return 1;
}
