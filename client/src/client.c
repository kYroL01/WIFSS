#include <client.h>


int main(void)
{
	int sock;
	struct sockaddr_in server;

	pthread_t sthread, cthread, ithread;

	if(init(&server, &sock))
	{
		MUTEX data;
		data.sock         = sock;
		data.keepGoing    = true;
		data.mutex        = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

		pthread_create(&sthread, NULL, &serverCommunication, (void*)&data);
		pthread_create(&cthread, NULL, &clientCommunication, (void*)&data);
		pthread_create(&ithread, NULL, &infiniteWaitingFnct, (void*)&data);

		pthread_join(ithread, NULL);

		pthread_cancel(cthread);
		pthread_cancel(sthread);

		pthread_mutex_destroy(&(data.mutex));
	}

	disconnect(sock);

	return 0;
}
