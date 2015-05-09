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
			char path[BUFFER] = {0};
			sscanf(buff, "upload %s", _path);
			printf("Server is asking us to upload: %s\n", path);
			upload(path, sock);
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
