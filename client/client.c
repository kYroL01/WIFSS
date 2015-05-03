#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define BUFFER 256
#define ENDSIG "quit"


#define true  1
#define false 0
typedef int bool;




int beginWith(char *str, char *cmd)
{
	short int __i, lenght = strlen(str);

	for(__i = 0; __i < lenght && str[__i] != ' '; __i++)
	{
		str[__i] = tolower(str[__i]);

		if(str[__i] != cmd[__i])
		{
			return 0;
		}
	}

	return 1;
}


void upload(char *path, int sock)
{
	FILE *__file = NULL;
	__file = fopen(path, "r");

	if(__file == NULL)
	{
		printf("%s file asked is unreachable.\n", path);
	}

	else
	{
		printf("Sending of %s file is started.\n", path);

		char *__buff;
		rewind(__file);

		while(ftell(__file) != SEEK_END)									//... tant que l'on n'est pas à la fin du fichier en question
		{
			fgets(hash, BUFFER, __file); 									//On récupère la ligne actuelle

			while(send(sock, __buff, strlen(__buff), 0) != strlen(__buff)); //... tant qu'elle n'est pas partie convenablement
			
			fseek(__file, SEEK_CUR, SEEK_CUR + BUFFER); 					//On se positionne une taille de Buffer plus loin pour le prochain envoi
			memset(__buff, 0, BUFFER); 										//On réinitialise proprement le Buffer
		}

		fclose(__file);
	}
}



int main(int argc, char const *argv[])
{
	char buff[BUFFER];
	short int port;

	printf("\n\033[32mStarting client...\033[0m\n");

	do
	{
		printf("\n\033[1;0mServer IP: \033[0m");
		fgets(buff, BUFFER, stdin);

	} while(inet_buff(buff) != INADDR_NONE);

	do
	{
		printf("\n\033[1;0mPort: \033[0m");
		scanf("%hd", &port);

	} while(port > 65535 || port < 1024);

	struct sockaddr_in SERVER;
	SERVER.sin_family      = AF_INET;
	SERVER.sin_port        = htons(port);
	SERVER.sin_addr.s_addr = inet_addr(buff);

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	short int value = 1;
	while(value != 0)
	{
		value = connect(sock, (const struct sockaddr*)&SERVER, sizeof(SERVER));

		if(value != 0)
		{
			printf("\n\n\033[31mErreur during connexion to %s:%d.\nLet's retry.\033[0m\n\n", buff, port);
			sleep(3);
		}
	}
	
	bool keepGoing = true;
	while(keepGoing)
	{
		memset(buff, 0, BUFFER);

		printf("|: ");
		fgets(buff, BUFFER, stdin);

		//...

		//cmd_thread
		//snd_thread
		//rcv_thread


		if(!strcmp(buff, ENDSIG))
		{
			keepGoing = false;
			printf("\n\nLet's close connexion with Server...");
		}
	}


	close(sock);
	printf("\n\033[35mConnexion successfully closed.\033[35m\n\n");


	return 1;
}
