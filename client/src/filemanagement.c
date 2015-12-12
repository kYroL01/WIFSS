#include <client.h>

_Bool setWorkDir(void)
{
	system("clear");

	printf("\n\033[32m[WIFSS] Starting Client...\033[0m\n");


	char *_temp = NULL;

	_temp = (char*)malloc((strlen("WORKDIR=") + strlen(getenv("HOME")) + strlen(PATHWORKINGDIR)) * sizeof(*_temp));
	sprintf(_temp, "WORKDIR=%s%s", getenv("HOME"), PATHWORKINGDIR);

	if(putenv(_temp) != 0)
	{
		free(_temp);
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
	char _buff[BUFFER]           = "";
	char _destFile[PATHSIZE]     = ""; 
	char _fileName[PATHSIZE / 4] = "";

	sscanf(command, "ispresent %[^\n]", _fileName);

	strcpy(_destFile, getenv("WORKDIR"));
	strcat(_destFile, _fileName);

	if(!access(_destFile, F_OK))
	{
		sprintf(_buff, "%s", PRESENT);
	}
	else
	{
		sprintf(_buff, "%s", FAIL);
	}

	send(sock, _buff, BUFFER, false);
}

void removeFile(const char *command)
{
	char _fileName[PATHSIZE / 4] = "";
	char _destFile[PATHSIZE]     = ""; 

	sscanf(command, "remove %[^\n]", _fileName);

	strcpy(_destFile, getenv("WORKDIR"));
	strcat(_destFile, _fileName);

	if(remove(_destFile) == -1)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be removed as well.\033[0m\n\n", _fileName);
	}
	else
	{
		printf("\n\033[32m[WIFSS] \"%s\" has been removed.\033[0m\n\n", _fileName);
	}
}

void renameFile(const char *command)
{
	char _fileName[PATHSIZE / 4]    = "";
	char _newFileName[PATHSIZE / 4] = "";
	char _destFile[BUFFER]          = "";
	char _newDestFile[BUFFER]       = "";

	sscanf(command, "rename %s %[^\n]", _fileName, _newFileName);

	strcpy(_destFile, getenv("WORKDIR"));
	strcat(_destFile, _fileName);
	strcpy(_newDestFile, getenv("WORKDIR"));
	strcat(_newDestFile, _newFileName);

	if(rename(_destFile, _newDestFile) == -1)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be renamed as well.\033[0m\n\n", _fileName);
	}
	else
	{
		printf("\n\033[32m[WIFSS] \"%s\" has been renamed as \"%s\".\033[0m\n\n", _fileName, _newFileName);
	}
}

void listFiles(char *_buff)
{
	char _destDir[PATHSIZE] = "";

	strcpy(_destDir, getenv("WORKDIR"));

	DIR *directory = opendir(_destDir);

	if(directory != NULL)
	{
		if(_buff != NULL)
		{
			memset(_buff, 0, BUFFER);
			strcpy(_buff, "list: ");
		}

		struct dirent *ep;

		while((ep = readdir(directory)))
		{
			if(strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..")) /* All bar currentDir + parentDir */
			{
				if(_buff != NULL) /* If function called with a buff, non-verbose list */
				{
					strcat(_buff, ep->d_name);
					strcat(_buff, "/");
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
		printf("\n\033[31m[WIFSS] Error: Couldn't open the target directory: \"%s\".\033[0m\n", _destDir);
	}
}


void asklist(const char *command, int sock)
{
	char _buff[BUFFER] = "";

	send(sock, command, BUFFER, false);
	recv(sock, _buff, BUFFER, false);

	if(str_beginwith(_buff, LIST))
	{
		char _temp[BUFFER] = "";
		sscanf(_buff, "list: %[^\n]", _temp);

		printf("\n\n[WIFSS] List of client asked:\n\t");
		short int _i;
		for(_i = 0; _temp[_i] != '\0'; _i++)
		{
			if(_temp[_i] == '/')
			{
				printf("\n\t");
				continue;
			}
			else
			{
				printf("%c", _temp[_i]);
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
	char _destDir[2 * PATHSIZE] = "";
	short int _count = 0;

	strcpy(_destDir, getenv("WORKDIR"));
	DIR *directory = opendir(_destDir);

	if(directory == NULL)
	{
		printf("\n\033[31m[WIFSS] Error: Target directory doesn\'t exist, we attempt to create it.\033[0m\n");
		mkdir(_destDir, S_IRWXU);
		directory = opendir(_destDir);

		if(directory == NULL)
		{
			printf("\n\033[31m[WIFSS] Error: Target directory couldn\'t be created.\033[0m\n\n");
			perror("Error");
			return false;
		}
		else
		{
			printf("\n\033[32m[WIFSS] Target directory: \"%s\" successfully created.\033[0m\n", _destDir);
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

			memset(_destDir, 0, PATHSIZE);
			strcpy(_destDir, getenv("WORKDIR"));
			strcat(_destDir, ep->d_name);

			if(stat(_destDir, &file_stat) != 0)
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

			_count++;
		}
	}

	closedir(directory);
	
	if(_count > MAXFILEDIR)
	{
		printf("\n\033[32m[WIFSS] Error: You've got more than %d files in: \"%s\" directory. Clean up this please.\033[0m\n", MAXFILEDIR, _destDir);
		return false;
	}

	return true;
}