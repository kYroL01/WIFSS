#include <client.h>



int upload(const char *path, int idClient, int sock)
{
	if(!s_Com(ASKFILE, sock, path, idClient))
	{
		printf("Client %d does\'nt have %s file...\n", idClient, path);
		return -1;
	}

	FILE *_file = NULL;

	uint32_t _fsize = 0;

	printf("Trying to upload %s to Server...\n", path);
	_file = fopen(path, "rb");

	if(_file == NULL)
	{
		printf("%s file asked is unreachable.\n", path);
		return -1;
	}

	fseek(_file, 0, SEEK_END);
	_fsize = ftell(_file);
	fseek(_file, 0, 0);

	printf("Sending: %d bytes of %s\n", _fsize, path);

	char _buff[BUFFER];

	while(ftell(_file) != SEEK_END)
	{
		memset(_buff, 0, BUFFER);

		fread(_buff, sizeof(char), BUFFER, _file);

		while(send(sock, _buff, strlen(_buff), false) != (ssize_t)strlen(_buff));
	}

	fclose(_file);

	return 1;
}
