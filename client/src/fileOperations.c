#include <client.h>

_Bool setWorkDir(void)
{
	system("clear");

	printf("\n\033[32m[WIFSS] Starting Client...\033[0m\n");


	char *temp = NULL;

	temp = (char*)malloc((strlen("WORKDIR=") + strlen(getenv("HOME")) + strlen(PATHWORKINGDIR)) * sizeof(*temp));
	sprintf(temp, "WORKDIR=%s%s", getenv("HOME"), PATHWORKINGDIR);

	if(putenv(temp) != 0)
	{
		free(temp);
		printf("\n\033[31m[WIFSS] Error: Working directory couldn\'t be set as environment variable.\033[0m\n\n");
		return false;
	}
	else
	{
		return true;
	}
}

void isPresent(const char *command, int sock)
{
	char buff[BUFFER]           = "";
	char destFile[PATHSIZE]     = ""; 
	char fileName[PATHSIZE / 4] = "";

	sscanf(command, "ispresent %[^\n]", fileName);

	strcpy(destFile, getenv("WORKDIR"));
	strcat(destFile, fileName);

	if(!access(destFile, F_OK))
	{
		sprintf(buff, "%s", PRESENT);
	}
	else
	{
		sprintf(buff, "%s", FAIL);
	}

	send(sock, buff, BUFFER, false);
}

void removeFile(const char *command)
{
	char fileName[PATHSIZE / 4] = "";
	char destFile[PATHSIZE]     = ""; 

	sscanf(command, "remove %[^\n]", fileName);

	strcpy(destFile, getenv("WORKDIR"));
	strcat(destFile, fileName);

	if(remove(destFile) == -1)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be removed as well.\033[0m\n\n", fileName);
	}
	else
	{
		printf("\n\033[32m[WIFSS] \"%s\" has been removed.\033[0m\n\n", fileName);
	}
}

void renameFile(const char *command)
{
	char fileName[PATHSIZE / 4]    = "";
	char newFileName[PATHSIZE / 4] = "";
	char destFile[BUFFER]          = "";
	char newDestFile[BUFFER]       = "";

	sscanf(command, "rename %s %[^\n]", fileName, newFileName);

	strcpy(destFile, getenv("WORKDIR"));
	strcat(destFile, fileName);
	strcpy(newDestFile, getenv("WORKDIR"));
	strcat(newDestFile, newFileName);

	if(rename(destFile, newDestFile) == -1)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be renamed as well.\033[0m\n\n", fileName);
	}
	else
	{
		printf("\n\033[32m[WIFSS] \"%s\" has been renamed as \"%s\".\033[0m\n\n", fileName, newFileName);
	}
}

void listFiles(char *buff)
{
	char destDir[PATHSIZE] = "";

	strcpy(destDir, getenv("WORKDIR"));

	DIR *directory = opendir(destDir);

	if(directory != NULL)
	{
		if(buff != NULL)
		{
			memset(buff, 0, BUFFER);
			strcpy(buff, "list: ");
		}

		struct dirent *ep;

		while((ep = readdir(directory)))
		{
			if(strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..")) /* All bar currentDir + parentDir */
			{
				if(buff != NULL) /* If function called with a buff, non-verbose list */
				{
					strcat(buff, ep->d_name);
					strcat(buff, "/");
				}
				else
				{
					printf("\t%s\n", ep->d_name);
				}
			}
		}

		printf("\n");

		closedir(directory);
	}

	else
	{
		printf("\n\033[31m[WIFSS] Error: Couldn't open the target directory: \"%s\".\033[0m\n", destDir);
	}
}

void who(int sock)
{
	char buff[BUFFER] = "";

	sprintf(buff, "%s", "who");
	send(sock, buff, BUFFER, false);
	memset(buff, 0, BUFFER);
}

void askList(const char *command, int sock)
{
	char buff[BUFFER] = "";

	send(sock, command, BUFFER, false);
	recv(sock, buff, BUFFER, false);

	if(str_beginwith(buff, LIST))
	{
		char temp[BUFFER] = "";
		sscanf(buff, "list: %[^\n]", temp);

		printf("\n\n[WIFSS] File list of client asked:\n\t");
		short int i;
		for(i = 0; temp[i] != '\0'; i++)
		{
			if(temp[i] == '/')
			{
				printf("\n\t");
				continue;
			}
			else
			{
				printf("%c", temp[i]);
			}
		}
		printf("\n");
	}
	else
	{
		printf("\n\n\033[33m[WIFSS] Error: List of client asked could not be retrieved as well.\033[0m\n\t");
	}
}


_Bool checkDownloadFolder()
{
	char destDir[2 * PATHSIZE] = "";
	short int count = 0;

	strcpy(destDir, getenv("WORKDIR"));
	DIR *directory = opendir(destDir);

	if(directory == NULL)
	{
		printf("\n\033[31m[WIFSS] Error: Target directory doesn\'t exist, we attempt to create it.\033[0m\n");
		mkdir(destDir, S_IRWXU);
		directory = opendir(destDir);

		if(directory == NULL)
		{
			printf("\n\033[31m[WIFSS] Error: Target directory couldn\'t be created.\033[0m\n\n");
			perror("Error");
			return false;
		}
		else
		{
			printf("\n\033[32m[WIFSS] Target directory: \"%s\" successfully created.\033[0m\n", destDir);
		}
	}

	struct dirent *ep;
	struct stat file_stat;

	while((ep = readdir(directory)))
	{
		if(strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..")) /* All bar currentDir + parentDir */
		{
			if((int)strlen(ep->d_name) > MAXFILENAME)
			{
				printf("\n\033[31m[WIFSS] Error: One of your file (\"%s\") in your working directory has more than %d characters in its name. Consider rename it.\033[0m\n\n", ep->d_name, MAXFILENAME);
				return false;
			}

			memset(destDir, 0, PATHSIZE);
			strcpy(destDir, getenv("WORKDIR"));
			strcat(destDir, ep->d_name);

			if(stat(destDir, &file_stat) != 0)
			{
				perror("Error");
				printf("\n\033[31m[WIFSS] Error: A problem occured during reading information about one of your working directory content.\033[0m\n\n");
				return false;
			}
			else
			{
				if(S_ISDIR(file_stat.st_mode))
				{
					printf("\n\033[31m[WIFSS] Error: One of your working directory content is a sub-directory (\"%s\"). Please consider remove it.\033[0m\n\n", ep->d_name);
					return false;
				}
			}

			count++;
		}
	}

	closedir(directory);
	
	if(count > MAXFILEDIR)
	{
		printf("\n\033[32m[WIFSS] Error: You've got more than %d files in: \"%s\" directory. Clean up this please.\033[0m\n", MAXFILEDIR, destDir);
		return false;
	}

	printf("\n\033[32m[WIFSS] Your directory is clear and you\'re able to start downloading or uploading some files.\033[0m\n\n");
	return true;
}