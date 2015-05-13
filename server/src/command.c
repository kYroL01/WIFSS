#include <server.h>

void* command_handler(void* data)
{
	char buffer[BSIZE] = "";
	bool running = true;
	int i;

	while(running)
	{
		scanf("%s", buffer);
		
		if(str_beginwith(buffer, "STOP"))
		{
			broadcast(SID, "[WIFSS] Server is going to shutdown.\n");
			close_all_connections();
			for(i = 0; i < MAX_CLIENTS; i++)
			{
				pthread_cancel(threads[i]);
			}
			
			running = false;
		}
	}
	
	printf("[WIFSS] Server stopped.\n");
	exit(false);
	return data;
}
