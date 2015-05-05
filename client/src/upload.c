#include <client.h>

int upload(const char *path, int sock)
{
	FILE *file = NULL;
	uint32_t fsize = 0;

	printf("Trying to upload : %s\n", path);
	file = fopen(path, "rb");

	if(file == NULL)
	{
		printf("%s file asked is unreachable.\n", path);
		return 1;
	}

	/* calcul de la taille */
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, 0);
	
	printf("File size is: %d\n", fsize);
	
	if(fsize > BUFFER) {
		/* Fichier plus gros que Buffer -> découpage donc il faudra revoir l'algo ci-dessous ... */
	}
	
	else
	{
		printf("Sending: %s\n", path);

		char buff[BUFFER];
		/* rewind(__file); */

		while(ftell(file) != SEEK_END)
		{
			fgets(buff, BUFFER, file);

			while(send(sock, buff, strlen(buff), false) != strlen(buff));

			/* fseek(file, SEEK_CUR, SEEK_CUR + BUFFER); 
			   fgets se déplace déjà non ? */
			memset(buff, 0, BUFFER);
		}

		fclose(file);
	}
}
