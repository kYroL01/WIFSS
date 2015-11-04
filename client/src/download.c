#include <client.h>

void download(const char *command, int sock)
{
	char _fileName[PATHSIZE / 4] = "";
	char _destFile[PATHSIZE]     = "";
	char _temp[PATHSIZE]         = "";
	const char *_dotPart         = ".part";
	const char *_path            = "/Downloads/WIFSS/";

	short int _foo = -1;
	sscanf(command, "download %hd %[^\n]", &_foo, _fileName);

	strcpy(_destFile, getenv("HOME")); //_destFile = "$HOME"
	strcat(_destFile, _path);          //_destFile = "$HOME/Downloads/WIFSS/"
	strcat(_destFile, _fileName);      //_destFile = "$HOME/Downloads/WIFSS/fileName"

	FILE *_file = NULL;

	if(access(_destFile, F_OK))
	{
		_file = fopen(_destFile, "wb");
	}

	if(_file == NULL)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" cannot be created. Already exist ?\033[0m\n\n", _fileName);
		return;
	}
	else
	{
		strcpy(_temp, _destFile);
		strcat(_temp, _dotPart); //We append ".part" at the end of the file name during download

		if(rename(_destFile, _temp) == -1)
		{
			printf("\n\033[31m[WIFSS] Error: \"%s\" could not be renamed as well.\033[0m\n\n", _path);
			closeFile(_file, _fileName);
			return;
		}
	}

	char _buff[BUFFER] = "";

	send(sock, command, BUFFER, false); //Wake up the server with the command
	/* Waiting for ACK... */
	recv(sock, _buff, BUFFER, false);
	if(!strcmp(_buff, FAIL))
	{
		closeFile(_file, _fileName);
		if(remove(_destFile) == -1)
		{
			printf("\n\033[31m[WIFSS] Error: \"%s\" could not be removed as well.\033[0m\n", _fileName);
		}

		return;
	}

	printf("\n\033[32m[WIFSS] Reception of \"%s\" started in \"~%s/\" !\033[0m\n\n", _fileName, _path);

	int res;
	while(1)
	{
		do
		{
			res = recv(sock, _buff, BUFFER, false);
			if(res <= 0)
			{
				printf("\n\033[31m[WIFSS] Error: \"%s\" could not be downloaded completely.\033[0m\n", _fileName);
				closeFile(_file, _fileName);
				return;
			}

		} while(res != BUFFER);

		if(!strcmp(_buff, FINISHED))
		{
			break;
		}
		else
		{
			fwrite(_buff, sizeof(char), BUFFER, _file);
			fseek(_file, SEEK_CUR, SEEK_CUR + BUFFER);
			memset(_buff, 0, BUFFER);
		}
	}

	if(rename(_temp, _destFile) == -1)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be renamed as well.\033[0m\n\n", _fileName);
	}

	closeFile(_file, _fileName);
}

void closeFile(FILE *file, const char *fileName)
{
	if(fclose(file) == EOF)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" could not be closed as well.\033[0m\n\n", fileName);
	}
}
