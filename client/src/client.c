#include <client.h>

int main(void)
{
	int sock = -1;
	struct sockaddr_in server;

	pthread_t sthread, cthread;

	if(setWorkDir() && checkDownloadFolder() && start(&server, &sock))
	{
		DATA data;
		data.sock    = sock;
		data.sthread = &sthread;
		data.cthread = &cthread;

		pthread_create(&sthread, NULL, &serverCommunication, (void*)&data);
		pthread_create(&cthread, NULL, &clientCommunication, (void*)&data);

		pthread_join(sthread, NULL);
		pthread_join(cthread, NULL);
	}

	stop(sock);

	return 0;
}
