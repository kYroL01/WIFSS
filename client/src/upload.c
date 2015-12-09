#include <client.h>

void upload(const char *command, int sock)
{
	long _fsize              = 0;
	char _buff[BUFFER]       = "";
	char _fileName[PATHSIZE] = "";
	char _destFile[PATHSIZE] = "";

	sscanf(command, "upload %[^\n]", _fileName);
	printf("\n\n[sthread] [Server] is asking us to upload: \"%s\". Trying to upload it...\n", _fileName);

	strcpy(_destFile, getenv("WORKDIR")); //_destFile = "$HOME/Downloads/WIFSS/"
	strcat(_destFile, _fileName);      //_destFile = "$HOME/Downloads/WIFSS/fileName"

	FILE *_file = NULL;

	if(!access(_destFile, F_OK))
	{
		_file = fopen(_destFile, "rb");
	}

	if(_file == NULL)
	{
		printf("\n\033[31m[WIFSS] \"%s\" asked is unreachable.\033[0m\n", _fileName);
		sprintf(_buff, "%s", FAIL);
		send(sock, _buff, BUFFER, false);
		return;
	}
	else
	{
		memset(_buff, 0, BUFFER);

		fseek(_file, 0, SEEK_END);
		_fsize = ftell(_file);
		fseek(_file, 0, SEEK_SET);

		sprintf(_buff, "size: %ld", _fsize);
		send(sock, _buff, BUFFER, false);
		printf("\n[WIFSS] Sending: \"%s\" (%ld bytes).\n", _fileName, _fsize);
	}

	recv(sock, _buff, BUFFER, false); //Receive "OK", cue-role

	int _res;
	while(ftell(_file) != SEEK_END)
	{
		memset(_buff, 0, BUFFER);
		fread(_buff, sizeof(char), BUFFER, _file);

		do
		{
			_res = send(sock, _buff, BUFFER, false);
			if(_res <= 0)
			{
				printf("\n\n[WIFSS] File could not be uploaded completely.\n");
				break;
			}

		} while(_res != BUFFER);
	}

	memset(_buff, 0, BUFFER);
	sprintf(_buff, "%s", ENDT);
	send(sock, _buff, BUFFER, false);

	closeFile(_file, _fileName);
}
