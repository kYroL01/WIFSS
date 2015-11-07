#include <server.h>

void broadcast(int sender, const char *msg)
{
	for(short int _i = 0; _i < MAX_CLIENTS; _i++)
	{
		if(_i != sender)
		{
			send(g_clients[_i].sock, msg, BUFFER, false);
		}
	}
}

void close_all_connections()
{
	printf("\n[WIFSS] Closing all connections...");

	char _buffer[BUFFER] = "";
	sprintf(_buffer, "%s", DISCONNECT);

	for(short int _i = 0; _i < MAX_CLIENTS; _i++)
	{
		if(g_clients[_i].status == TAKEN)
		{
			send(g_clients[_i].sock, _buffer, BUFFER, false);
		}
		printf(".");
	}
	printf(" done.\n");
}
