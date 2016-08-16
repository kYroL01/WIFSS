#include <server.h>

void broadcast(int sender, const char *msg)
{
	short int i;
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(i != sender)
		{
			send(g_clients[i].sock, msg, BUFFER, false);
		}
	}
}

void who(int sender)
{
	char buffer[BUFFER] = "Id(s) of other(s) client(s) currently connected: ";
	char microBuff[8];

	short int i;
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(g_clients[i].status == TAKEN && i != sender)
		{
			memset(microBuff, 0, 8);
			sprintf(microBuff, "\t%d", i);
			strcat(buffer, microBuff);
		}
	}

	if(sender >= 0)
	{
		printf("\n\n[WIFSS] [Client %d] has just listed others connected clients.\n\n", sender);
		send(g_clients[sender].sock, buffer, BUFFER, false);
	}

	else
	{
		printf("\n\n[WIFSS] %s\n\n", buffer);
	}
}

void close_all_connections()
{
	printf("\n[WIFSS] Closing all connections...");

	char buffer[BUFFER] = "";
	sprintf(buffer, "%s", DISCONNECT);

	short int i;
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(g_clients[i].status == TAKEN)
		{
			send(g_clients[i].sock, buffer, BUFFER, false);
		}
		printf(".");
	}
	printf(" done.\n");
}
