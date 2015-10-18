#include <server.h>

void* command_handler(void* data)
{
	char buffer[BSIZE] = "";
	bool running = true;
	int i;
	printf("\n");

	while(running)
	{
		printf("|: ");
		fgets(buffer, BSIZE, stdin);
		str_removeSlashN(buffer);
	
		if(str_beginwith(buffer, "STOP") || str_beginwith(buffer, "stop"))
		{
			broadcast(SID, "Server is going to shutdown.\n");
			close_all_connections();
			for(i = 0; i < MAX_CLIENTS; i++)
			{
				pthread_cancel(threads[i]);
			}
			
			running = false;
		}
	}
	
	printf("[WIFSS] Server stopped.\n");
	for(short int _i = 0; _i < 60; _i++)
	{
		printf("=");
	}
	printf("\n\n");
	exit(false);
	return data;
}
