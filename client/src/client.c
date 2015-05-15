#include <client.h>



int main(void)
{
	int sock;
	struct sockaddr_in SERVER;

	pthread_t sthread, cthread;

	if(init(&SERVER, &sock) == true)
	{		
		pthread_create(&sthread, NULL, &scom, (void*)&sock);
		
		pthread_create(&cthread, NULL, &clientCommand, (void*)&sock);
	}

	pthread_join(sthread, NULL);
	pthread_join(cthread, NULL);

	disconnect(sock);

	return 1;
}
