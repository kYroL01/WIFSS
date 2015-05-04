#include <header.h>




int beginWith(char *str, char *cmd)
{
	short int __i, lenght = strlen(str);

	for(__i = 0; __i < lenght && str[__i] != ' '; __i++)
	{
		str[__i] = tolower(str[__i]);

		if(str[__i] != cmd[__i])
		{
			return 0;
		}
	}

	return 1;
}


void upload(char *path, int sock)
{
	FILE *__file = NULL;

	__file = fopen(path, "rb");

	if(__file == NULL)
	{
		printf("%s file asked is unreachable.\n", path);
	}

	else
	{
		printf("Sending of %s file is started.\n", path);

		char __buff[BUFFER];
		rewind(__file);

		while(ftell(__file) != SEEK_END)
		{
			fgets(__buff, BUFFER, __file);

			while(send(sock, __buff, strlen(__buff), false) != strlen(__buff));

			fseek(__file, SEEK_CUR, SEEK_CUR + BUFFER);
			memset(__buff, 0, BUFFER);
		}

		fclose(__file);
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
