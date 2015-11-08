#include <server.h>

void asklist(const char *command, int sender_id)
{
	char _buff[BUFFER]   = "";
	short int _remote_id = -1;

	sscanf(command, "asklist %hd", &_remote_id);

	if(g_clients[_remote_id].status == TAKEN && sender_id != _remote_id && _remote_id >= 0 && _remote_id < MAX_CLIENTS)
	{
		sprintf(_buff, "%s", ASKLIST);
		send(g_clients[_remote_id].sock, _buff, BUFFER, false);
		/* Waiting for file list... */
		memset(_buff, 0, BUFFER);
		recv(g_clients[_remote_id].sock, _buff, BUFFER, false);
		send(g_clients[sender_id].sock, _buff, BUFFER, false);
		printf("\n\n[Client %d] asked the file list of [Client %d].\n\n", sender_id, _remote_id);
	}
	else
	{
		sprintf(_buff, "%s", "Error: This client is not connected or its identifier is invalid.");
		send(g_clients[sender_id].sock, _buff, BUFFER, false);
		printf("\n\n[Client %d] asked the file list of [Client %d], but he is not connected.\n\n", sender_id, _remote_id);
	}
}
