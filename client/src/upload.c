#include <client.h>

int upload(const char *path, int sock)
{
	long _fsize        = 0;
	char _buff[BUFFER] = "";
	FILE *_file        = NULL;

	printf("\n[WIFSS] Trying to upload %s to Server...\n", path);
	_file = fopen(path, "rb");

	if(_file == NULL)
	{
		printf("\n[WIFSS] %s file asked is unreachable.\n", path);
		sprintf(_buff, "%s", FAIL)
		send(sock, _buff, BUFFER, false);
		return 0;
	}

	fseek(_file, 0, SEEK_END);
	_fsize = ftell(_file);
	fseek(_file, 0, SEEK_SET);

	printf("\n[WIFSS] Sending: %ld bytes of \"%s\".\n", _fsize, path);

	sprintf(_buff, "size: %ld", _fsize);
	send(sock, _buff, BUFFER, false);

	while(ftell(_file) != SEEK_END)
	{
		memset(_buff, 0, BUFFER);
		fread(_buff, sizeof(char), BUFFER, _file);

		while(send(sock, _buff, BUFFER, false) != BUFFER);
	}
	
	send(sock, ENDT, strlen(ENDT), false);

	fclose(_file);

	return 1;
}
