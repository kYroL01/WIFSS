#include <stdio.h>
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
typedef bool int;


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
	SERVER.sin_addr.s_addr = inet_buff(buff);

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	short int value = 1;
	while(value != 0)
	{
		value = connect(sock, (const struct sockaddr*)&SERVER, sizeof(SERVER));

		if(value != 0)
		{
			printf("\n\n\033[31mErreur during connexion to %s:%d.\nLet's retry.\033[0m\n\n", buff, PORT);
			sleep(3);
		}
	}
	
	bool keepGoing = true;
	while(keepGoing)
	{
		memset(buff, 0, BUFFER);

		printf("|: ");
		fgets(buff, BUFFER, stdin);

		/* ... */

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
