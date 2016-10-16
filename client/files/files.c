#include "files.h"


bool set_work_dir(void)
{
	g_core_variables.working_dir = calloc(strlen(getenv("HOME")) + strlen(PATHWORKINGDIR), sizeof(*(g_core_variables.working_dir)));

	if(g_core_variables.working_dir == NULL)
	{
		printf("\n\033[31m[WIFSS] Error: Can\'t allocate memory for the environment variable \"WORKING_DIR\".\033[0m\n\n");
		return false;
	}

	strcpy(g_core_variables.working_dir, getenv("HOME"));
	strcat(g_core_variables.working_dir, PATHWORKINGDIR);

	return true;
}


bool check_download_folder()
{
	char destDir[4 * PATHSIZE];
	uint8_t count = 0;

	strcpy(destDir, g_core_variables.working_dir);
	DIR *directory = opendir(destDir);


	if(directory == NULL)
	{
		printf("\n\033[31m[WIFSS] Error: Target directory doesn\'t exist, we attempt to create it.\033[0m\n\n");
		mkdir(destDir, S_IRWXU);
		directory = opendir(destDir);

		if(directory == NULL)
		{
			printf("\n\033[31m[WIFSS] Error: Target directory couldn\'t be created.\033[0m\n\n");
			return false;
		}
		else
		{
			printf("\n\033[32m[WIFSS] Target directory: \"%s\" successfully created.\033[0m\n\n", destDir);
		}
	}

	struct dirent *ep;
	struct stat file_stat;

	while((ep = readdir(directory)))
	{
		/* All the content, bar 'current_dir' and 'parent_dir' */
		if(strcmp(ep->d_name, ".") && strcmp(ep->d_name, ".."))
		{
			if(strlen(ep->d_name) > (size_t)MAXFILENAME)
			{
				printf("\n\033[31m[WIFSS] Error: One of your file (\"%s\") in your working directory has more than %d characters in its name. Consider renaming it.\033[0m\n\n", ep->d_name, MAXFILENAME);
				return false;
			}

			strcpy(destDir, g_core_variables.working_dir);
			strcat(destDir, ep->d_name);

			if(stat(destDir, &file_stat) == -1)
			{
				printf("\n\033[31m[WIFSS] Error: A problem occurred during reading information about one of your working directory content (\"%s\").\033[0m\n\n", destDir);
				return false;
			}

			else
			{
				if(S_ISDIR(file_stat.st_mode))
				{
					printf("\n\033[31m[WIFSS] Error: One of your working directory content is a sub-directory (\"%s\"). Please consider removing it.\033[0m\n\n", ep->d_name);
					return false;
				}
			}

			count++;
		}
	}

	closedir(directory);
	
	if(count > MAXFILEDIR)
	{
		printf("\n\033[31m[WIFSS] Error: You've got more than %d files in: \"%s\" directory. Clean this up please.\033[0m\n\n", MAXFILEDIR, destDir);
		return false;
	}

	else
	{
		printf("\n\033[32m[WIFSS] Your directory is clear and you\'re able to start downloading or uploading some files.\033[0m\n\n");
		return true;
	}
}


void is_present(const char *command)
{
	char buff[BUFFER];

	char destFile[PATHSIZE]     = ""; 
	char fileName[PATHSIZE / 4] = "";

	sscanf(command, "ispresent %[^\n]", fileName);

	strcpy(destFile, g_core_variables.working_dir);
	strcat(destFile, fileName);

	if(!access(destFile, F_OK))
	{
		strcpy(buff, PRESENT);
	}

	else
	{
		strcpy(buff, FAIL);
	}

	send(g_core_variables.server_sock, buff, BUFFER, false);
}


void remove_file(const char *command)
{
	char fileName[PATHSIZE / 4] = "";
	char destFile[PATHSIZE]     = ""; 

	sscanf(command, "remove %[^\n]", fileName);

	strcpy(destFile, g_core_variables.working_dir);
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


void rename_file(const char *command)
{
	char fileName[PATHSIZE / 4]    = "";
	char newFileName[PATHSIZE / 4] = "";
	char destFile[BUFFER];
	char newDestFile[BUFFER];

	sscanf(command, "rename %s %[^\n]", fileName, newFileName);

	strcpy(destFile, g_core_variables.working_dir);
	strcat(destFile, fileName);
	strcpy(newDestFile, g_core_variables.working_dir);
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


void close_file(FILE *file, const char *fileName)
{
	if(fclose(file) == EOF)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be closed as well.\033[0m\n\n", fileName);
	}
}


void list_files(char *buffer)
{
	DIR *directory = opendir(g_core_variables.working_dir);

	if(directory != NULL)
	{
		/* Let's pre-format the buffer for the server */
		if(buffer != NULL)
		{
			strcpy(buffer, "list: ");
		}

		struct dirent *ep;
		while((ep = readdir(directory)))
		{
			/* All the content, bar 'current_dir' and 'parent_dir' */
			if(strcmp(ep->d_name, ".") && strcmp(ep->d_name, ".."))
			{
				/* If the function is called with a buffer: non-verbose listing */
				if(buffer != NULL)
				{
					strcat(buffer, ep->d_name);
					strcat(buffer, "/");
				}

				else
				{
					printf("\t%s\n", ep->d_name);
				}
			}
		}

		printf("\n");

		if(closedir(directory) != 0)
		{
			printf("\n\033[31m[WIFSS] Error: Couldn't close the target directory: \"%s\".\033[0m\n\n", g_core_variables.working_dir);		
		}
	}

	else
	{
		printf("\n\033[31m[WIFSS] Error: Couldn't open the target directory: \"%s\".\033[0m\n\n", g_core_variables.working_dir);
	}
}


void ask_list(const char *command)
{
	char buff[BUFFER] = "";

	send(g_core_variables.server_sock, command, BUFFER, false);
	recv(g_core_variables.server_sock, buff, BUFFER, false);

	if(str_beginwith(buff, LIST))
	{
		char temp[BUFFER] = "";
		sscanf(buff, "list: %[^\n]", temp);

		printf("\n\n[WIFSS] File list of client asked:\n\t");

		for(uint16_t i = 0; temp[i] != '\0'; i++)
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
		printf("\n\n\033[33m[WIFSS] Error: List of client asked could not be retrieved as well.\033[0m\n\n");
	}
}
