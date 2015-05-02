#include <server.h>

void* command_handler(void* data)
{
	char buffer[BSIZE] = "";
	int running = 1;
	int i;

	while(running)
	{
		scanf("%s", buffer);
		
		if(str_beginwith(buffer, "STOP")) {
			broadcast(SID, "[WIFSS] Server is going to shutdown.\n");
			close_all_connections();
			for(i=0; i<MAX_CLIENTS; i++)
			{
				pthread_cancel(threads[i]);
			}
			
			running = 0;
		}
	}
	
	printf("[WIFSS] Server stopped.\n");
	exit(0);
	return data;
}
