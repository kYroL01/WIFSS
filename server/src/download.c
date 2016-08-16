#include <server.h>

void download(const char *command, int sender_id)
{
	char cpy[BUFFER]      = "";
	char buffer[BUFFER]   = "";
	char filename[BUFFER] = "";
	long int fsize        =  0;
	short int remote_id   = -1;
	int res_DL = 0, res_UL = 0;
	struct sockaddr_in clientDL, clientUL;
	int clientDL_sock, clientUL_sock;
	unsigned int asize;

	sscanf(command, "download %hd %[^\n]", &remote_id, filename);
	printf("\n\n[Client %d] is asking the uploading of [Client %d]'s \"%s\".\n", sender_id, remote_id, filename);

	if((remote_id >= MAX_CLIENTS) || (remote_id < 0) || (remote_id == sender_id) || (g_clients[remote_id].sock <= 0))
	{
		memset(filename, 0, BUFFER);
		sprintf(filename, "%s", "Error: Client wanted is invalid or not connected...");
		send(g_clients[sender_id].sock, filename, BUFFER, false);
		return;
	}

	/* Let's create a new socket for the client who want to download */

	clientDL_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	clientDL.sin_family      = AF_INET;
	clientDL.sin_addr.s_addr = INADDR_ANY;
	clientDL.sin_port        = htons(SERVER_PORT + sender_id + 1); /* We use this value because it's known by client as well */

	res_DL = bind(SERVER_PORT + sender_id, (struct sockaddr*)&clientDL, sizeof(clientDL));

	/* Now the same, but with the client who'll upload the file */

	clientUL_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	clientUL.sin_family      = AF_INET;
	clientUL.sin_addr.s_addr = INADDR_ANY;
	clientUL.sin_port        = htons(SERVER_PORT + remote_id + 1);

	res_UL = bind(SERVER_PORT + remote_id, (struct sockaddr*)&clientUL, sizeof(clientUL));

	close(clientDL_sock);
	close(clientUL_sock);

	/* Let's test the bindings */

	if(res_DL < 0 || res_UL < 0)
	{
		printf("\n\n\033[31m[WIFSS] Error during creation of the listening socket for one of the two clients (%d) & (%hd) [bind (%d) & (%d)].\033[0m\n", sender_id, remote_id, res_DL, res_UL);
		memset(buffer, 0, BUFFER);
		sprintf(buffer, "%s", FAIL);
		send(g_clients[sender_id].sock, buffer, BUFFER, false);
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

	sprintf(cpy, "upload %s", filename);
	send(g_clients[remote_id].sock, cpy, BUFFER, false);
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
