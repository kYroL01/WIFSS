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

void closeServer(int listen_socket)
{
	close_all_connections();

	if(close(listen_socket) == -1)
	{
		printf("\n\033[35m[WIFSS] Socket of server couldn't be successfully closed.\033[0m\n");
	}
	else
	{
		printf("\n\033[32m[WIFSS] Socket of server successfully closed.\033[0m\n");
	}

	for(short int _i = 0; _i < MAX_CLIENTS; _i++)
	{
		pthread_cancel(threads[_i]);
	}

	printf("[WIFSS] Server stopped.\n");
	for(short int _k = 0; _k < 60; _k++)
	{
		printf("=");
	}
	printf("\n\n");
}
