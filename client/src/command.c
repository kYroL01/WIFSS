#include <client.h>

int str_beginwith(const char *w, const char *s)
{
	while(*s)
	{
		if(*s != *w)
		{
			return 0;
		}

		w++;
		s++;
	}

	return 1;
}

int str_validation(const char *str, short int nbArgs)
{
	short int _i, _arg;

	for(_i = 0, _arg = 1; _i < BUFFER; _i++)
	{
		if(str[_i] == ' ' && (str[_i + 1] != ' ' && str[_i + 1] != '\0'))
		{
			_arg++;
		}
	}

	if(_arg > nbArgs)
	{
		printf("\nToo many arguments for this command.\n\n");
		return 0;
	}

	else if (_arg < nbArgs)
	{
		printf("\nToo few arguments for this command.\n\n");
		return 0;
	}

	else
	{
		return 1;
	}
}

void* scom(void *data)
{
	int sock;
	int result;
	char buff[BUFFER] = {0};
	
	sock = *((int*)data);
	
	bool keepGoing = true;
	while(keepGoing)
	{
		memset(buff, 0, BUFFER);
		result = recv(sock, buff, BUFFER, 0);
		
		/* Ici on reçoit des choses du serveur */
		if(str_beginwith(buff, UPLOAD))
		{
			/* Le serv demande d'upload un fichier 
				on l'envoie au serveur via la fonction upload */
			char _path[BUFFER] = {0};
			sscanf(buff, "upload %s", _path);
			printf("Server is asking us to upload: %s\n", _path);
			upload(_path, sock);
		}
		
		if(result <= 0)
		{
			//keepGoing = false;
			return NULL;
		}

		printf("[sthread] received from server: %s\n", buff);
	}
	
	return NULL;
}

void handle_command(const char *command, int sock, bool *connected)
{
	if(!strcmp(command, QUIT))
	{
		printf("\n\nLet's close connection with Server...");
		*connected = false;
	}

	else if(str_beginwith(command, DOWNLOAD) && str_validation(command, ARGDWL))
	{
		char _path[32] = {0};

		sscanf(command, "download %s", _path);

		if(!download(command, sock))
		{
			printf("File couldn't be downloaded correctly.\n\n");
		}
		else
		{
			printf("File downloaded !\n\n");
		}
	}

	else if(str_beginwith(command, TUNNEL) && str_validation(command, ARGTUN))
	{
		int idClient = 0;

		sscanf(command, "tunnel %d", &idClient);

		tunneling(sock, idClient);
	}

	else
	{
		printf("\nCommand unknown.\n");
	}
}

void lowerCase(char *buff)
{
	short int _i;

	for(_i = 0; _i < (short int)strlen(buff) && buff[_i] != ' '; _i++)
	{
		buff[_i] = tolower(buff[_i]);
	}
}
