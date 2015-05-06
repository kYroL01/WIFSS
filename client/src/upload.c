#include <client.h>



int upload(const char *path, int idClient, int sock)
{
	if(!s_Com(ASKFILE, sock, path, indClient))
	{
		printf("Client %d does\'nt have %s file...\n", idClient, path);
		return -1;
	}

	FILE *__file = NULL;

	uint32_t __fsize = 0;

	printf("Trying to upload %s to Server...\n", path);
	__file = fopen(path, "rb");

	if(__file == NULL)
	{
		printf("%s file asked is unreachable.\n", path);
		return -1;
	}

	fseek(__file, 0, SEEK_END);
	__fsize = ftell(__file);
	fseek(__file, 0, 0);

	printf("Sending: %d bytes of %s\n", __fsize, path);

	char __buff[BUFFER];

	while(ftell(__file) != SEEK_END)
	{
		memset(__buff, 0, BUFFER);

		fread(__buff, sizeof(char), BUFFER, __file);

		while(send(sock, __buff, strlen(__buff), false) != strlen(__buff));
	}

	fclose(__file);
}
