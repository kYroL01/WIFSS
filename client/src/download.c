#include <client.h>

int download(const char *path, int sock)
{
	FILE *_file = NULL;

	if(!access(path, F_OK))
	{
		_file = fopen(path, "wb");
	}


	if(_file == NULL)
	{
		printf("%s file cannot be created.\n", path);
		return -1;
	}

	printf("Reception of %s file started.\n", path);

	char _buff[BUFFER];


	bool keepGoing = true;
	while(keepGoing)
	{
		memset(_buff, 0, BUFFER);
		
		while(recv(sock, _buff, BUFFER, false) != BUFFER)

		if(!strcmp(_buff, FINISHED))
		{
			keepGoing = false;
		}
		else
		{
			fwrite(_buff, BUFFER, BUFFER, _file);
		}

		fseek(_file, SEEK_CUR, SEEK_CUR + BUFFER);
	}

	fclose(_file);

	return 1;
}
