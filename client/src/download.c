#include <client.h>

void download(const char *command, int sock)
{
	char fileName[PATHSIZE / 4] = "";
	char destFile[PATHSIZE]     = "";
	char temp[PATHSIZE]         = "";
	const char *dotPart         = ".part";

	short int foo = -1;
	sscanf(command, "download %hd %[^\n]", &foo, fileName);

	strcpy(destFile, getenv("WORKDIR")); /* destFile = "$HOME/Downloads/WIFSS/" */
	strcat(destFile, fileName);      /* destFile = "$HOME/Downloads/WIFSS/fileName" */

	strcpy(temp, destFile);
	strcat(temp, dotPart); /* We append ".part" at the end of the file name during download */

	FILE *file = NULL;

	if(!access(temp, F_OK))
	{
		/* Continue interrupted download of last time ? */
	}

	if(access(destFile, F_OK))
	{
		file = fopen(destFile, "wb");
	}

	if(file == NULL)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" cannot be created. Already exists ?\033[0m\n\n", fileName);
		return;
	}
	else
	{
		if(rename(destFile, temp) == -1)
		{
			printf("\n\033[31m[WIFSS] Error: \"%s\" could not be renamed as well.\033[0m\n\n", fileName);
			closeFile(file, fileName);
			return;
		}
	}

	char buff[BUFFER] = "";
	struct sockaddr_in serverDL;
	int sockDL;
	int res;

	send(sock, command, BUFFER, false); /* Wake up the server with the command */

	/* Ok, now we'll need a new socket to download this file through the server */

	serverDL.sin_port        = htons(SERVER_PORT + CLIENT_ID + 1);
	serverDL.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	sockDL = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	res = connect(sockDL, (struct sockaddr*)&serverDL, sizeof(serverDL));
	if(res < 0)
	{
		printf("\n\033[31m[WIFSS] Error: could not establish a new connection the server.\033[0m\n\n");
		return;
	}

	/* Waiting for ACK... */
	recv(sockDL, buff, BUFFER, false);
	if(!strcmp(buff, FAIL))
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be uploaded by remote [Client %hd].\033[0m\n", fileName, foo);
		closeFile(file, fileName);
		if(remove(destFile) == -1)
		{
			printf("\n\033[31m[WIFSS] Error: \"%s\" could not be removed as well.\033[0m\n", fileName);
		}
		return;
	}
	else
	{
		long int fsize = 0;
		sscanf(buff, "size: %ld", &fsize);
		printf("\n\033[32m[WIFSS] Reception of \"%s\" (%ld bytes) started in \"%s\" !\033[0m\n\n", fileName, fsize, getenv("WORKDIR"));
	}

	while(1)
	{
		do
		{
			res = recv(sockDL, buff, BUFFER, false);
			if(res <= 0)
			{
				printf("\n\033[31m[WIFSS] Error: \"%s\" could not be downloaded completely.\033[0m\n", fileName);
				closeFile(file, fileName);
				return;
			}

		} while(res != BUFFER);

		if(!strcmp(buff, FINISHED))
		{
			printf("\n\033[32m[WIFSS] Download finished !\033[0m\n\n");
			break;
		}
		else
		{
			fwrite(buff, sizeof(char), BUFFER, file);
			fseek(file, SEEK_CUR, SEEK_CUR + BUFFER);
			memset(buff, 0, BUFFER);
		}
	}

	if(rename(temp, destFile) == -1)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be renamed as well.\033[0m\n\n", fileName);
	}

	closeFile(file, fileName);
}

void closeFile(FILE *file, const char *fileName)
{
	if(fclose(file) == EOF)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be closed as well.\033[0m\n\n", fileName);
	}
}
