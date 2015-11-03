#include <client.h>

int download(const char *command, int sock)
{
	send(data->sock, command, BUFFER, false);

	char _path[PATHSIZE] = "";

	/* Research of file name + extension */
	for(short int _k = strlen(command); _k > 0; _k++)
	{
		if(command[_k] == '/')
		{
			strcpy(_path, command + _k + 1); //We just copy the last chars of the string
			break;
		}
	}

	FILE *_file = NULL;

	if(!access(_path, F_OK))
	{
		_file = fopen(_path, "wb");
	}

	if(_file == NULL)
	{
		printf("\n\033[31m[WIFSS] Error: \"%s\" file cannot be created.\033[0m\n\n", _path);
		return 0;
	}

	printf("\n\033[32m[WFISS] Reception of \"%s\" file started !\033[0m\n", _path);

	char _buff[BUFFER] = "";
	_Bool keepGoing = true;
	do
	{
		while(recv(sock, _buff, BUFFER, false) != BUFFER);

		if(!strcmp(_buff, FINISHED))
		{
			keepGoing = false;
		}
		else
		{
			fwrite(_buff, BUFFER, 1, _file);
		}

		fseek(_file, SEEK_CUR, SEEK_CUR + BUFFER);

		memset(_buff, 0, BUFFER);

	} while(keepGoing);

	fclose(_file);

	return 1;
}
