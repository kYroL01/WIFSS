#include <client.h>



int main(void)
{
	int sock;
	struct sockaddr_in SERVER;

	pthread_t sthread, cthread;


	if(init(&SERVER, &sock))
	{
		MUTEX data;
		data.mutex        = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
		data.sock         = sock;
		data.keepGoing    = true;
		data.tunnelOpened = false;

		pthread_create(&sthread, NULL, &serverCommunication, (void*)&data);
		pthread_create(&cthread, NULL, &clientCommunication, (void*)&data);
		
		pthread_join(cthread, NULL);
	
		pthread_mutex_destroy(&(data.mutex));
	}


	disconnect(sock);

	return 0;
}
