#include <client.h>



int upload(const char *path, int sock)
{
	FILE *__file = NULL;

	uint32_t __fsize = 0;

	printf("Trying to upload : %s\n", path);
	__file = fopen(path, "rb");

	if(__file == NULL)
	{
		printf("%s file asked is unreachable.\n", path);
		return 0;
	}

	fseek(__file, 0, SEEK_END);
	__fsize = ftell(__file);
	fseek(__file, 0, 0);
	
	printf("File size is: %d.\n", __fsize);
	
	if(__fsize > BUFFER)
	{
		/* Fichier plus gros que Buffer -> découpage donc il faudra revoir l'algo ci-dessous ... */
	}
	
	else
	{
		printf("Sending: %s...\n", path);

		char __buff[BUFFER];

		while(ftell(__file) != SEEK_END)
		{
			memset(__buff, 0, BUFFER);

			fgets(__buff, BUFFER, __file);

			while(send(sock, __buff, strlen(__buff), false) != strlen(__buff));
		}

		fclose(__file);
	}
}
