#include <client.h>



int download(const char *path, int idClient, int sock)
{
	FILE *__file = NULL;

	if(!access(path, F_OK))
	{
		__file = fopen(path, "wb");
	}


	if(__file == NULL)
	{
		printf("%s file cannot be created.\n", path);
		return -1;
	}

	printf("Reception of %s file started.\n", path);

	char __buff[BUFFER];

	bool keepGoing = true;
	while(keepGoing)
	{
		while(recv(sock, __buff, BUFFER, false) != BUFFER)

		if(!strcmp(__buff, FINISHED))
		{
			keepGoing = false;
		}
		else
		{
			fwrite(__buff, BUFFER, BUFFER, __file);
		}

		fseek(__file, SEEK_CUR, SEEK_CUR + BUFFER);
		memset(__buff, 0, BUFFER);
	}

	fclose(__file);

	return 1;
}
