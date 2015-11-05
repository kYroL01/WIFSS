#include <server.h>

void disconnect(const char *buffer)
{
	short int _idTemp = -2;
	sscanf(buffer, "disconnect %hd", &_idTemp);
	if(_idTemp == -1)
	{
		close_all_connections(); 
		printf("\n");
	}
	else if(_idTemp >= 0 && _idTemp < MAX_CLIENTS)
	{
		if(g_clients[_idTemp].status == TAKEN)
		{
			send(g_clients[_idTemp].sock, DISCONNECT, BUFFER, false);
			close(g_clients[_idTemp].sock);
		}
		else
		{
			printf("\n[WIFSS] This client is offline yet. Can't disconnect him.\n\n");
		}
	}
	else
	{
		printf("\n[WIFSS] This client identifier is invalid.\n\n");
	}
}
