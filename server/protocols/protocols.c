#include "protocols.h"


void who(const int8_t sender)
{
	char buffer[BUFFER] = "Id(s) of other client(s) currently connected: ";
	char microBuff[8];

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		if(g_core_variables.clients[i].status == TAKEN && i != sender)
		{
			strncpy(microBuff, "", 8);
			snprintf(microBuff, 8, "\t%d", i);
			strcat(buffer, microBuff);
		}
	}

	if(sender >= 0)
	{
		printf("\n\n[WIFSS] [Client %d] has just listed others connected clients.\n\n", sender);
		SSL_write(g_core_variables.clients[sender].ssl, buffer, BUFFER);
	}

	else
	{
		printf("\n\n[WIFSS] %s\n\n", buffer);
	}
}


void message(const char *command, const uint8_t sender_id)
{
	char copy[BUFFER]   = "";
	char buffer[BUFFER] = "";

	strncpy(buffer, getSecondArgsGroup(command), BUFFER);
	sprintf(copy, "[Client %d] : \"%s\".", sender_id, buffer);
	broadcast(sender_id, copy);
	printf("\n\n[Client %d] says: \"%s\".\n\n", sender_id, buffer);
}


void whisper(const char *command, const uint8_t sender_id)
{
	char copy[BUFFER]   = "";
	char buffer[BUFFER] = "";

	int8_t idTemp = getSecondArgsGroupAsInteger(command);
	strncpy(copy, getThirdArgsGroup(command), BUFFER);

	if(g_core_variables.clients[idTemp].status == TAKEN && sender_id != idTemp && idTemp >= 0 && idTemp < MAX_CLIENTS)
	{
		sprintf(buffer, "[Client %d] whispers: \"%s\".", sender_id, copy);
		SSL_write(g_core_variables.clients[idTemp].ssl, buffer, BUFFER);
		printf("\n\n[Client %d] whispers to [Client %d]: \"%s\".\n\n", sender_id, idTemp, copy);
	}
	else
	{
		sprintf(buffer, "%s", "Error: This client is not connected or its identifier is invalid. Take a look to \"help\".");
		SSL_write(g_core_variables.clients[sender_id].ssl, buffer, BUFFER);
		printf("\n\n[Client %d] tried to whisper to [Client %d]: \"%s\", but he is not connected.\n\n", sender_id, idTemp, copy);
	}
}


void broadcast(const uint8_t sender, const char *msg)
{
	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		if(i != sender && g_core_variables.clients[i].status == TAKEN)
		{
			SSL_write(g_core_variables.clients[i].ssl, msg, BUFFER);
		}
	}
}


void disconnect(const char *buffer)
{
	int8_t idTemp = getSecondArgsGroupAsInteger(buffer);

	if(idTemp == -1)
	{
		close_all_connections(); 
		printf("\n");
	}

	else if(idTemp >= 0 && idTemp < MAX_CLIENTS)
	{
		if(g_core_variables.clients[idTemp].status == TAKEN)
		{
			SSL_write(g_core_variables.clients[idTemp].ssl, DISCONNECT, BUFFER);
			close(g_core_variables.clients[idTemp].sock);
		}
		else
		{
			printf("\n[WIFSS] This client is already offline. You can\'t disconnect him.\n\n");
		}
	}

	else
	{
		printf("\n[WIFSS] This client identifier is invalid.\n\n");
	}
}


void close_all_connections(void)
{
	printf("\n[WIFSS] Closing all connections...");

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		if(g_core_variables.clients[i].status == TAKEN)
		{
			SSL_write(g_core_variables.clients[i].ssl, DISCONNECT, BUFFER);
		}

		printf(".");
	}

	printf(" done.\n");
}
