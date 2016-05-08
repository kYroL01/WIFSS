#include <server.h>

void broadcast(int sender, const char *msg)
{
	short int _i;
	for(_i = 0; _i < MAX_CLIENTS; _i++)
	{
		if(_i != sender)
		{
			send(g_clients[_i].sock, msg, BUFFER, false);
		}
	}
}

void who(int sender)
{
	char _buffer[BUFFER] = "Id(s) of other(s) client(s) currently connected: ";
	char _microBuff[8];

	short int _i;
	for(_i = 0; _i < MAX_CLIENTS; _i++)
	{
		if(g_clients[_i].status == TAKEN && _i != sender)
		{
			memset(_microBuff, 0, 8);
			sprintf(_microBuff, "\t%d", _i);
			strcat(_buffer, _microBuff);
		}
	}

	if(sender >= 0)
	{
		printf("\n\n[WIFSS] [Client %d] has just listed others connected clients.\n\n", sender);
		send(g_clients[sender].sock, _buffer, BUFFER, false);
	}

	else
	{
		printf("\n\n[WIFSS] %s\n\n", _buffer);
	}
}

void close_all_connections()
{
	printf("\n[WIFSS] Closing all connections...");

	char _buffer[BUFFER] = "";
	sprintf(_buffer, "%s", DISCONNECT);

	short int _i;
	for(_i = 0; _i < MAX_CLIENTS; _i++)
	{
		if(g_clients[_i].status == TAKEN)
		{
			send(g_clients[_i].sock, _buffer, BUFFER, false);
		}
		printf(".");
	}
	printf(" done.\n");
}
