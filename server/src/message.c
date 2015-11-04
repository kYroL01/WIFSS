#include <server.h>

void messagep(const char *command, int sender_id)
{
	char _cpy[BUFFER]     = "";
	char _buffer[BUFFER]  = "";
	short int _idTemp = -1;

	sscanf(command, "sendp %hd %[^\n]", &_idTemp, _cpy);

	if(g_clients[_idTemp].status == TAKEN)
	{
		sprintf(_buffer, "[Client %d] whispers: \"%s\".", sender_id, _cpy);
		send(g_clients[_idTemp].sock, _buffer, BUFFER, false);
		printf("\n\n[Client %d] whispers to [Client %d]: \"%s\".\n\n", sender_id, _idTemp, _cpy);
	}
	else
	{
		sprintf(_buffer, "%s", "Error: Client not connected.");
		send(g_clients[sender_id].sock, _buffer, BUFFER, false);
		printf("\n\n[Client %d] tried to whisper to [Client %d]: \"%s\", but he is not connected.\n", sender_id, _idTemp, _cpy);
	}
}

void message(const char *command, int sender_id)
{
	char _cpy[BUFFER]     = "";
	char _buffer[BUFFER]  = "";

	sscanf(command, "send %[^\n]", _buffer);
	sprintf(_cpy, "[Client %d] : \"%s\".", sender_id, _buffer);
	broadcast(sender_id, _cpy);
	printf("\n\n[Client %d] says: \"%s\".\n\n", sender_id, _buffer);
}
