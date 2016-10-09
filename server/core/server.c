#include "server.h"


int main(int argc, const char *argv[])
{
	/* In order to deal with '-Wextra' GCC flag */
	(void)argc;
	(void)argv;

	start_server();

	return 0;
}
