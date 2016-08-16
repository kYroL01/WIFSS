#include <server.h>

void message(const char *command, int sender_id)
{
	char cpy[BUFFER]    = "";
	char buffer[BUFFER] = "";

	sscanf(command, "send %[^\n]", buffer);
	sprintf(cpy, "[Client %d] : \"%s\".", sender_id, buffer);
	broadcast(sender_id, cpy);
	printf("\n\n[Client %d] says: \"%s\".\n\n", sender_id, buffer);
}

void whisper(const char *command, int sender_id)
{
	char cpy[BUFFER]    = "";
	char buffer[BUFFER] = "";
	short int idTemp = -1;

	sscanf(command, "sendp %hd %[^\n]", &idTemp, cpy);

	if(g_clients[idTemp].status == TAKEN && sender_id != idTemp && idTemp >= 0 && idTemp < MAX_CLIENTS)
	{
		sprintf(buffer, "[Client %d] whispers: \"%s\".", sender_id, cpy);
		send(g_clients[idTemp].sock, buffer, BUFFER, false);
		printf("\n\n[Client %d] whispers to [Client %d]: \"%s\".\n\n", sender_id, idTemp, cpy);
	}
	else
	{
		sprintf(buffer, "%s", "Error: This client is not connected or its identifier is invalid.");
		send(g_clients[sender_id].sock, buffer, BUFFER, false);
		printf("\n\n[Client %d] tried to whisper to [Client %d]: \"%s\", but he is not connected.\n\n", sender_id, idTemp, cpy);
	}
}
