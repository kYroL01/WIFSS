#include "protocols.h"


void download(const char *command, const uint8_t sender_id)
{
	char copy[BUFFER]     = "";
	char buffer[BUFFER]   = "";
	char filename[BUFFER] = "";
	long int fsize        =  0;
	int8_t remote_id   = -1;
	int res_DL = 0, res_UL = 0;
	struct sockaddr_in clientDL, clientUL;
	int clientDL_sock, clientUL_sock;
	unsigned int asize;

	sscanf(command, "download %" SCNd8 "%[^\n]", &remote_id, filename);
	printf("\n\n[Client %d] is asking the uploading of [Client %d]'s \"%s\".\n", sender_id, remote_id, filename);

	if((remote_id >= MAX_CLIENTS) || (remote_id < 0) || (remote_id == sender_id) || (g_core_variables.clients[remote_id].sock <= 0))
	{
		memset(filename, 0, BUFFER);
		sprintf(filename, "%s", "Error: Client wanted is invalid or not connected...");
		send(g_core_variables.clients[sender_id].sock, filename, BUFFER, false);
		return;
	}

	/* Let's create a new socket for the client who want to download */

	clientDL_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	clientDL.sin_family      = AF_INET;
	clientDL.sin_addr.s_addr = INADDR_ANY;
	clientDL.sin_port        = htons(g_core_variables.server_port + sender_id + 1); /* We use this value because it's known by client as well */

	res_DL = bind(g_core_variables.server_port + sender_id, (struct sockaddr*)&clientDL, sizeof(clientDL));

	/* Now the same, but with the client who'll upload the file */

	clientUL_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	clientUL.sin_family      = AF_INET;
	clientUL.sin_addr.s_addr = INADDR_ANY;
	clientUL.sin_port        = htons(g_core_variables.server_port + remote_id + 1);

	res_UL = bind(g_core_variables.server_port + remote_id, (struct sockaddr*)&clientUL, sizeof(clientUL));

	close(clientDL_sock);
	close(clientUL_sock);

	/* Let's test the bindings */

	if(res_DL < 0 || res_UL < 0)
	{
		printf("\n\n\033[31m[WIFSS] Error during creation of the listening socket for one of the two clients (%d) & (%" SCNd8 ") [bind (%d) & (%d)].\033[0m\n", sender_id, remote_id, res_DL, res_UL);
		memset(buffer, 0, BUFFER);
		sprintf(buffer, "%s", FAIL);
		send(g_core_variables.clients[sender_id].sock, buffer, BUFFER, false);
		return;
	}

	listen(clientDL_sock, 1);
	listen(clientUL_sock, 1);

	asize = sizeof(struct sockaddr_in);

	/* Now waiting for both connections ! */

	res_DL = accept(clientDL_sock, (struct sockaddr*)&clientDL, &asize);
	printf("\n\n[WIFSS] Downloading client paired !.\n");
	close(clientDL_sock);

	res_UL = accept(clientUL_sock, (struct sockaddr*)&clientUL, &asize);
	printf("\n\n[WIFSS] Uploading client paired !.\n");
	close(clientUL_sock);

	/* We FINALLY could send a file ! */

	sprintf(copy, "upload %s", filename);
	send(g_core_variables.clients[remote_id].sock, copy, BUFFER, false);
	/* Waiting for ACK... */
	recv(res_UL, buffer, BUFFER, false);
	if(!strcmp(buffer, FAIL))
	{
		printf("\n\n[WIFSS] A problem occurred with the file during its opening.\n");
		memset(buffer, 0, BUFFER);
		sprintf(buffer, "%s", FAIL);
		send(res_DL, buffer, BUFFER, false);
		return;
	}
	else
	{
		sscanf(buffer, "size: %ld", &fsize);
		printf("\n\n[WIFSS] File size: %ld bytes.\n", fsize);
		send(res_DL, buffer, BUFFER, false);
		/* Waiting for ACK... */
		memset(buffer, 0, BUFFER);
		recv(res_DL, buffer, BUFFER, false);
		/* Receive and Send "OK" (cue-role), from sender, to remote */
		send(res_UL, buffer, BUFFER, false);
	}

	int res;
	while(1)
	{
		memset(buffer, 0, BUFFER);

		do
		{
			res = recv(res_UL, buffer, BUFFER, false);
			if(res <= 0)
			{
				printf("\n[WIFSS] File could not be downloaded completely.\n\n");
				return;
			}

		} while(res != BUFFER);

		if(!strcmp(buffer, FINISHED))
		{
			break;
		}
		else
		{
			do
			{
				res = send(res_DL, buffer, BUFFER, false);
				if(res <= 0)
				{
					printf("\n[WIFSS] File could not be uploaded completely.\n\n");
					return;
				}

			} while(res != BUFFER);
		}
	}
}


void who(const int8_t sender)
{
	char buffer[BUFFER] = "Id(s) of other(s) client(s) currently connected: ";
	char microBuff[8];

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		if(g_core_variables.clients[i].status == TAKEN && i != sender)
		{
			strcpy(microBuff, "");
			sprintf(microBuff, "\t%d", i);
			strcat(buffer, microBuff);
		}
	}

	if(sender >= 0)
	{
		printf("\n\n[WIFSS] [Client %d] has just listed others connected clients.\n\n", sender);
		send(g_core_variables.clients[sender].sock, buffer, BUFFER, false);
	}

	else
	{
		printf("\n\n[WIFSS] %s\n\n", buffer);
	}
}


void ask_list(const char *command, const uint8_t sender_id)
{
	char buff[BUFFER] = "";
	int8_t remote_id = -1;

	sscanf(command, "asklist %" SCNd8, &remote_id);

	if(g_core_variables.clients[remote_id].status == TAKEN && sender_id != remote_id && remote_id >= 0 && remote_id < MAX_CLIENTS)
	{
		sprintf(buff, "%s", ASKLIST);
		send(g_core_variables.clients[remote_id].sock, buff, BUFFER, false);
		/* Waiting for file list... */
		memset(buff, 0, BUFFER);
		recv(g_core_variables.clients[remote_id].sock, buff, BUFFER, false);
		send(g_core_variables.clients[sender_id].sock, buff, BUFFER, false);
		printf("\n\n[Client %d] asked the file list of [Client %d].\n\n", sender_id, remote_id);
	}
	else
	{
		sprintf(buff, "%s", "Error: This client is not connected or its identifier is invalid.");
		send(g_core_variables.clients[sender_id].sock, buff, BUFFER, false);
		printf("\n\n[Client %d] asked the file list of [Client %d], but he is not connected.\n\n", sender_id, remote_id);
	}
}


void is_present(const char *command, const uint8_t sender_id)
{
	/* ... */
}


void message(const char *command, const uint8_t sender_id)
{
	char copy[BUFFER]    = "";
	char buffer[BUFFER] = "";

	sscanf(command, "send %[^\n]", buffer);
	sprintf(copy, "[Client %d] : \"%s\".", sender_id, buffer);
	broadcast(sender_id, copy);
	printf("\n\n[Client %d] says: \"%s\".\n\n", sender_id, buffer);
}


void whisper(const char *command, const uint8_t sender_id)
{
	char copy[BUFFER]    = "";
	char buffer[BUFFER] = "";
	int8_t idTemp = -1;

	sscanf(command, "sendp %" SCNd8 "%[^\n]", &idTemp, copy);

	if(g_core_variables.clients[idTemp].status == TAKEN && sender_id != idTemp && idTemp >= 0 && idTemp < MAX_CLIENTS)
	{
		sprintf(buffer, "[Client %d] whispers: \"%s\".", sender_id, copy);
		send(g_core_variables.clients[idTemp].sock, buffer, BUFFER, false);
		printf("\n\n[Client %d] whispers to [Client %d]: \"%s\".\n\n", sender_id, idTemp, copy);
	}
	else
	{
		sprintf(buffer, "%s", "Error: This client is not connected or its identifier is invalid.");
		send(g_core_variables.clients[sender_id].sock, buffer, BUFFER, false);
		printf("\n\n[Client %d] tried to whisper to [Client %d]: \"%s\", but he is not connected.\n\n", sender_id, idTemp, copy);
	}
}


void broadcast(const uint8_t sender, const char *msg)
{
	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		if(i != sender)
		{
			send(g_core_variables.clients[i].sock, msg, BUFFER, false);
		}
	}
}


void disconnect(const char *buffer)
{
	int8_t idTemp = -2;
	sscanf(buffer, "disconnect %" SCNd8, &idTemp);
	if(idTemp == -1)
	{
		close_all_connections(); 
		printf("\n");
	}
	else if(idTemp >= 0 && idTemp < MAX_CLIENTS)
	{
		if(g_core_variables.clients[idTemp].status == TAKEN)
		{
			send(g_core_variables.clients[idTemp].sock, DISCONNECT, BUFFER, false);
			close(g_core_variables.clients[idTemp].sock);
		}
		else
		{
			printf("\n[WIFSS] This client is already offline. Can't disconnect him.\n\n");
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
			send(g_core_variables.clients[i].sock, DISCONNECT, BUFFER, false);
		}

		printf(".");
	}

	printf(" done.\n");
}
