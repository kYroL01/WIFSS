#include <client.h>

void upload(const char *command, int sock)
{
	long fsize              = 0;
	char buff[BUFFER]       = "";
	char fileName[PATHSIZE] = "";
	char destFile[PATHSIZE] = "";

	sscanf(command, "upload %[^\n]", fileName);
	printf("\n\n[sthread] [Server] is asking us to upload: \"%s\". Trying to upload it...\n", fileName);

	strcpy(destFile, getenv("WORKDIR")); /* destFile = "$HOME/Downloads/WIFSS/" */
	strcat(destFile, fileName);      /* destFile = "$HOME/Downloads/WIFSS/fileName" */

	FILE *file = NULL;

	if(!access(destFile, F_OK))
	{
		file = fopen(destFile, "rb");
	}

	struct sockaddr_in serverDL;
	int sockDL;
	int res;

	/* Ok, now we'll need a new socket to upload this file through the server */

	serverDL.sin_port        = htons(SERVER_PORT + CLIENT_ID + 1);
	serverDL.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	sockDL = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	res = connect(sockDL, (struct sockaddr*)&serverDL, sizeof(serverDL));
	if(res < 0)
	{
		printf("\n\033[31m[WIFSS] Error: Could not establish a new connection the server.\033[0m\n\n");
		return;
	}

	if(file == NULL)
	{
		printf("\n\033[31m[WIFSS] \"%s\" asked is unreachable.\033[0m\n", fileName);
		sprintf(buff, "%s", FAIL);
		send(sockDL, buff, BUFFER, false);
	}
	else
	{
		memset(buff, 0, BUFFER);

		fseek(file, 0, SEEK_END);
		fsize = ftell(file);
		fseek(file, 0, SEEK_SET);

		sprintf(buff, "size: %ld", fsize);
		send(sockDL, buff, BUFFER, false);
		printf("\n[WIFSS] Sending: \"%s\" (%ld bytes).\n", fileName, fsize);

		recv(sockDL, buff, BUFFER, false); /* Receive "OK", cue-role */

		while(ftell(file) != SEEK_END)
		{
			memset(buff, 0, BUFFER);
			fread(buff, sizeof(char), BUFFER, file);

			do
			{
				res = send(sockDL, buff, BUFFER, false);
				if(res <= 0)
				{
					printf("\n\n[WIFSS] File could not be uploaded completely.\n");
					break;
				}

			} while(res != BUFFER);
		}

		memset(buff, 0, BUFFER);
		sprintf(buff, "%s", FINISHED);
		send(sockDL, buff, BUFFER, false);

		closeFile(file, fileName);
	}
}
