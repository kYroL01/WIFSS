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
			broadcast(SID, "[WIFSS] Le serveur va être arrêté.\n");
			close_all_connections();
			for(i=0; i<MAX_CLIENTS; i++)
			{
				pthread_cancel(threads[i]);
			}
			
			running = 0;
		}
	}
	
	printf("[WIFSS] Serveur arrêté.\n");
	exit(0);
	return data;
}
