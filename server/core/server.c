#include "server.h"


int main(int argc, const char *argv[])
{
	/* In order to deal with '-Wextra' "Unused variable" warning */
	(void)argc;
	(void)argv;

	start_server();

	pthread_create(&core_variables.command_thread, NULL, &command_handler, NULL);
	pthread_create(&core_variables.connections_thread, NULL, &connections_handler, NULL);

	pthread_join(core_variables.command_thread, NULL);
	pthread_cancel(core_variables.connections_thread);

	close_server();

	return 0;
}
