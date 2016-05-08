#include <server.h>

void download(const char *command, int sender_id)
{
	char _cpy[BUFFER]      = "";
	char _buffer[BUFFER]   = "";
	char _filename[BUFFER] = "";
	long int _fsize        =  0;
	short int _remote_id   = -1;

	sscanf(command, "download %hd %[^\n]", &_remote_id, _filename);
	printf("\n\n[Client %d] is asking the uploading of [Client %d]'s \"%s\".\n", sender_id, _remote_id, _filename);

	if((_remote_id >= MAX_CLIENTS) || (_remote_id < 0) || (_remote_id == sender_id) || (g_clients[_remote_id].sock <= 0))
	{
		memset(_filename, 0, BUFFER);
		sprintf(_filename, "%s", "Error: Client wanted is invalid or not conected...");
		send(g_clients[sender_id].sock, _filename, BUFFER, false);
		return;
	}

	sprintf(_cpy, "upload %s", _filename);
	send(g_clients[_remote_id].sock, _cpy, BUFFER, false); /* LEAVES THE FUNCTION HERE ## BUG ## */
	/* Waiting for ACK... */
	recv(g_clients[_remote_id].sock, _buffer, BUFFER, false);
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
		send(g_clients[sender_id].sock, _buffer, BUFFER, false);
		/* Waiting for ACK... */
		memset(_buffer, 0, BUFFER);
		recv(g_clients[sender_id].sock, _buffer, BUFFER, false);
		/* Receive and Send "OK" (cue-role), from sender, to remote */
		send(g_clients[_remote_id].sock, _buffer, BUFFER, false);
	}

	int _res;
	while(1)
	{
		memset(_buffer, 0, BUFFER);

		do
		{
			_res = recv(g_clients[_remote_id].sock, _buffer, BUFFER, false);
			if(_res <= 0)
			{
				printf("\n[WIFSS] File could not be downloaded completely.\n\n");
				return;
			}

		} while(_res != BUFFER);

		if(!strcmp(_buffer, ENDT))
		{
			break;
		}
		else
		{
			do
			{
				_res = send(g_clients[sender_id].sock, _buffer, BUFFER, false);
				if(_res <= 0)
				{
					printf("\n[WIFSS] File could not be uploaded completely.\n\n");
					return;
				}

			} while(_res != BUFFER);
		}
	}
}
