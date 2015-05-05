#include <client.h>

int str_beginwith(const char *w,const char *s)
{
	while(*s) {
		if(*s != *w) {
			return 0;
		}
		w++;
		s++;
	}
	return 1;
}

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
	fisze = ftell(file);
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


void download(char *path, int sock)
{
	FILE *__file = NULL;

	if(!access(path, F_OK)) //Check existence of the file before download one with the same name
	{
		__file = fopen(path, "wb");
	}


	if(__file == NULL)
	{
		printf("%s file cannot be created.\n", path);
	}

	else
	{
		printf("Reception of %s file started.\n", path);

		char __buff[BUFFER];
		rewind(__file);

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

		close(__file);
	}
}


int serverCommunication(int cmd, int sock, char *path = NULL)
{
	char *__buff[BUFFER];

	switch(cmd)
	{
		case ENDSIG:
			send(sock, END, strlen(END), false);
			recv(sock, __buff, BUFFER, false);
			if(!strcmp(__buff, "stop"))
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		case ASKFILE:
			strcpy(__buff, ISPRESENT); //Formatage du buffer... "ispresent path_file"
			strcat(__buff, " ");
			strcat(__buff, path);

			send(sock, __buff, BUFFER, false);
			sleep(1);
			recv(sock, __buff, BUFFER, false);
			if(!strcmp(__buff, PRESENT))
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		case FILEDWL:
			send(sock, FINISHED, strlen(FINISHED), false);
			recv(sock, __buff, BUFFER, false);
			if(!strcmp(__buff, OK))
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		default:
			printf("Unknown signal.\n");
			return 0;
			break;
	}
}
