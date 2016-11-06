#include "protocols.h"


void download(const char *command)
{
	char fileName[sysconf(_PC_NAME_MAX)];
	char destFile[strlen(g_core_variables.working_dir) + sysconf(_PC_NAME_MAX)];
	char temp[strlen(g_core_variables.working_dir) + sysconf(_PC_NAME_MAX) + strlen(DOTPART)];

	int8_t foo = -1;
	sscanf(command, "download %" SCNd8 " %[^\n]", &foo, fileName);

	strcpy(destFile, g_core_variables.working_dir);
	strcat(destFile, fileName);      /* destFile = "$HOME/Downloads/WIFSS/fileName" */

	strcpy(temp, destFile);
	strcat(temp, DOTPART); /* We append ".part" at the end of the file name during download */

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
			close_file(file, fileName);
			return;
		}
	}

	char buff[BUFFER] = "";
	struct sockaddr_in serverDL;
	int sockDL;
	int res;

	send(g_core_variables.server_port, command, BUFFER, false); /* Wake up the server with the command */

	/* Ok, now we'll need a new socket to download this file through the server */

	serverDL.sin_port        = htons(g_core_variables.server_port + g_core_variables.client_id + 1);
	serverDL.sin_addr.s_addr = inet_addr(g_core_variables.server_addr);

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
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be uploaded by remote [Client %" SCNu8 "].\033[0m\n", fileName, foo);
		close_file(file, fileName);
		if(remove(destFile) == -1)
		{
			printf("\n\033[31m[WIFSS] Error: \"%s\" could not be removed as well.\033[0m\n", fileName);
		}
		return;
	}
	else
	{
		int32_t fsize = 0;
		sscanf(buff, "size: %" SCNd32, &fsize);
		printf("\n\033[32m[WIFSS] Reception of \"%s\" (%" SCNd32 "bytes) started in \"%s\" !\033[0m\n\n", fileName, fsize, getenv("WORKDIR"));
	}

	while(1)
	{
		do
		{
			res = recv(sockDL, buff, BUFFER, false);
			if(res <= 0)
			{
				printf("\n\033[31m[WIFSS] Error: \"%s\" could not be downloaded completely.\033[0m\n", fileName);
				close_file(file, fileName);
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

	close_file(file, fileName);
}


void upload(const char *command)
{
	char buff[BUFFER] = "";
	char fileName[sysconf(_PC_NAME_MAX)];
	char destFile[strlen(g_core_variables.working_dir) + sysconf(_PC_NAME_MAX)];

	sscanf(command, "upload %[^\n]", fileName);
	printf("\n\n[sthread] [Server] is asking us to upload: \"%s\". Trying to upload it...\n", fileName);

	strcpy(destFile, g_core_variables.working_dir); /* destFile = "$HOME/Downloads/WIFSS/" */
	strcat(destFile, fileName);      /* destFile = "$HOME/Downloads/WIFSS/fileName" */

	FILE *file = NULL;

	if(!access(destFile, F_OK))
	{
		file = fopen(destFile, "rb");
	}

	struct sockaddr_in serverDL;

	/* Ok, now we'll need a new socket to upload this file through the server */

	serverDL.sin_port        = htons(g_core_variables.server_port + g_core_variables.client_id + 1);
	serverDL.sin_addr.s_addr = inet_addr(g_core_variables.server_addr);

	int8_t sockDL = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int16_t res = connect(sockDL, (struct sockaddr*)&serverDL, sizeof(serverDL));
	if(res == -1)
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

	else // À refaire complètement...
	{
		strcpy(buff, "");

		fseek(file, 0, SEEK_END);

		int32_t fsize = ftell(file);

		fseek(file, 0, SEEK_SET);

		sprintf(buff, "size: %" SCNd32, fsize);
		send(sockDL, buff, BUFFER, false);
		printf("\n[WIFSS] Sending: \"%s\" (%" SCNd32 " bytes).\n", fileName, fsize);

		recv(sockDL, buff, BUFFER, false); /* Receive "OK", cue-role */

		while(ftell(file) != SEEK_END)
		{
			strcpy(buff, "");
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

		strcpy(buff, "");
		sprintf(buff, "%s", FINISHED);
		send(sockDL, buff, BUFFER, false);

		close_file(file, fileName);
	}
}
