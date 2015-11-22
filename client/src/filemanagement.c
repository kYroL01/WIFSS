#include <client.h>

void isPresent(const char *command, int sock)
{
	char _buff[BUFFER]           = "";
	char _destFile[PATHSIZE]     = ""; 
	char _fileName[PATHSIZE / 4] = "";
	const char _path[PATHSIZE]   = "/Downloads/WIFSS/";

	sscanf(command, "ispresent %[^\n]", _fileName);

	strcpy(_destFile, getenv("HOME"));
	strcat(_destFile, _path);
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
	const char _path[PATHSIZE]   = "/Downloads/WIFSS/";

	sscanf(command, "remove %[^\n]", _fileName);

	strcpy(_destFile, getenv("HOME"));
	strcat(_destFile, _path);
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
	const char _path[PATHSIZE]      = "/Downloads/WIFSS/";

	sscanf(command, "rename %s %[^\n]", _fileName, _newFileName);

	strcpy(_destFile, getenv("HOME"));
	strcpy(_newDestFile, getenv("HOME"));
	strcat(_destFile, _path);
	strcat(_newDestFile, _path);
	strcat(_destFile, _fileName);
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
	char _destDir[PATHSIZE]    = "";
	const char _path[PATHSIZE] = "/Downloads/WIFSS/";

	strcpy(_destDir, getenv("HOME"));
	strcat(_destDir, _path);

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
			if(strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..")) //All bar currentDir + parentDir
			{
				if(_buff != NULL) //If function called with a buff, non-verbose list
				{
					strcat(_buff, ep->d_name);
					strcat(_buff, "/");
				}
				else
				{
					printf("\t");
					puts(ep->d_name);
				}
			}
		}

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
		for(short int _i = 0; _temp[_i] != '\0'; _i++)
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
	char _destDir[PATHSIZE]    = "";
	const char _path[PATHSIZE] = "/Downloads/WIFSS/";
	short int _count = 0;

	strcpy(_destDir, getenv("HOME"));
	strcat(_destDir, _path);

	DIR *directory = opendir(_destDir);

	if(directory == NULL)
	{
		printf("\n\033[31m[WIFSS] Error: Target directory doesn\'t exist, we attempt to create it.\033[0m\n");
		mkdir(_destDir, S_IRWXU);
		directory = opendir(_destDir);

		if(directory == NULL)
		{
			printf("\n\033[31m[WIFSS] Error: Target directory couldn\'t be created. Stopping now.\033[0m\n");
			return false;
		}
		else
		{
			printf("\n\033[32m[WIFSS] Target directory: \"%s\" successfully created.\033[0m\n", _destDir);
		}
	}

	struct dirent *ep;

	while((ep = readdir(directory)))
	{
		if(strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..")) //All bar currentDir + parentDir
		{
			_count++;
		}
	}

	closedir(directory);
	
	if(_count > MAXFILEDIR)
	{
		printf("\n\033[32m[WIFSS] Error: You've got so many files in: \"%s\" directory. Stopping now.\033[0m\n", _destDir);
		return false;
	}

	return true;
}