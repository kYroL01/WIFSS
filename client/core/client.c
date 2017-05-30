#include "client.h"


int main(int argc, const char *argv[])
{
	/* In order to deal with '-Wextra' "Unused variable" warning */
	(void)argc;
	(void)argv;

	pthread_t sthread, cthread;

	if(start_client())
	{
		Threads threads;
		threads.sthread = &sthread;
		threads.cthread = &cthread;

		pthread_create(&sthread, NULL, &server_communication, (void*)&threads);
		pthread_create(&cthread, NULL, &client_communication, (void*)&threads);

		pthread_join(sthread, NULL);
		pthread_join(cthread, NULL);
	}

	stop_client();

	return 0;
}
