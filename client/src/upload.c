#include <client.h>

int upload(const char *path, int sock)
{
	FILE *_file = NULL;
	char _buff[BUFFER];
	long _fsize = 0;

	printf("Trying to upload %s to Server...\n", path);
	_file = fopen(path, "rb");

	if(_file == NULL)
	{
		printf("%s file asked is unreachable.\n", path);
		send(sock, FAIL, strlen(FAIL), false);
		return 0;
	}

	fseek(_file, 0, SEEK_END);
	_fsize = ftell(_file);
	fseek(_file, 0, 0);

	printf("Sending: %ld bytes of %s\n", _fsize, path);

	sprintf(_buff, "size: %ld", _fsize);

	send(sock, _buff, BUFFER, false);


	while(ftell(_file) != SEEK_END)
	{
		memset(_buff, 0, BUFFER);

		fread(_buff, sizeof(char), BUFFER, _file);

		while(send(sock, _buff, strlen(_buff), false) != (int)strlen(_buff));
	}
	
	send(sock, ENDT, strlen(ENDT), false);

	fclose(_file);

	return 1;
}
