#include <server.h>

void askList(const char *command, int sender_id)
{
	char buff[BUFFER]   = "";
	short int remote_id = -1;

	sscanf(command, "asklist %hd", &remote_id);

	if(g_clients[remote_id].status == TAKEN && sender_id != remote_id && remote_id >= 0 && remote_id < MAX_CLIENTS)
	{
		sprintf(buff, "%s", ASKLIST);
		send(g_clients[remote_id].sock, buff, BUFFER, false);
		/* Waiting for file list... */
		memset(buff, 0, BUFFER);
		recv(g_clients[remote_id].sock, buff, BUFFER, false);
		send(g_clients[sender_id].sock, buff, BUFFER, false);
		printf("\n\n[Client %d] asked the file list of [Client %d].\n\n", sender_id, remote_id);
	}
	else
	{
		sprintf(buff, "%s", "Error: This client is not connected or its identifier is invalid.");
		send(g_clients[sender_id].sock, buff, BUFFER, false);
		printf("\n\n[Client %d] asked the file list of [Client %d], but he is not connected.\n\n", sender_id, remote_id);
	}
}

void isPresent(const char *command, int sender_id)
{
	/* ... */
}