#include <server.h>

void download(const char *command, int sender_id)
{
	char _cpy[BUFFER]     = "";
	char _buffer[BUFFER]  = "";
	char filename[BUFFER] = "";
	long int _fsize       =  0;
	short int remote_id   = -1;

	sscanf(command, "download %hd %[^\n]", &remote_id, filename);
	printf("\n\n[Client %d] is asking the uploading of [Client %d]'s \"%s\".\n\n", sender_id, remote_id, filename);

	if((remote_id > (MAX_CLIENTS - 1)) || (remote_id < 0) || (remote_id == sender_id))
	{
		memset(filename, 0, BUFFER);
		sprintf(filename, "%s", "Error: Client wanted is invalid...");
		send(g_clients[sender_id].sock, filename, BUFFER, false);
		return;
	}

	if(g_clients[remote_id].sock <= 0)
	{
		memset(filename, 0, BUFFER);
		sprintf(filename, "%s", "Error: Client asked is not connected...");
		send(g_clients[sender_id].sock, filename, BUFFER, false);
		return;
	}

	sprintf(_cpy, "upload %s", filename);
	send(g_clients[remote_id].sock, _cpy, BUFFER, false);
	/* Waiting for ACK... */
	recv(g_clients[remote_id].sock, _buffer, BUFFER, false);
	if(!strcmp(_buffer, FAIL))
	{
		printf("\n\n[WIFSS] A problem occured with the file during its opening.\n");
		memset(_buffer, 0, BUFFER);
		sprintf(_buffer, "%s", FAIL);
		send(g_clients[sender_id].sock, _buffer, BUFFER, false);
		return;
	}
	else
	{
		sscanf(_buffer, "size: %ld", &_fsize);
		printf("\n\n[WIFSS] File size: %ld bytes.\n", _fsize);
		memset(_buffer, 0, BUFFER);
		sprintf(_buffer, "%s", OK);
		send(g_clients[sender_id].sock, _buffer, BUFFER, false);
	}

	int res;
	while(1)
	{
		memset(_buffer, 0, BUFFER);

		do
		{
			res = recv(g_clients[remote_id].sock, _buffer, BUFFER, false);
			if(res <= 0)
			{
				printf("\n\n[WIFSS] File could not be downloaded completely.\n");
				break;
			}

		} while(res != BUFFER);

		if(!strcmp(_buffer, ENDT))
		{
			break;
		}
		else
		{
			do
			{
				res = send(g_clients[sender_id].sock, _buffer, BUFFER, false);
				if(res <= 0)
				{
					printf("\n\n[WIFSS] File could not be downloaded completely.\n");
					break;
				}

			} while(res != BUFFER);
		}
	}
}
