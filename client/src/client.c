#include <client.h>



int main(int argc, char const *argv[])
{
	char buff[BUFFER];
	short int port;

	printf("\n\033[32mStarting client...\033[0m\n");

	printf("\n\033[1;0mServer IP: \033[0m");
	fgets(buff, BUFFER, stdin);

	do
	{
		printf("\033[1;0mPort: \033[0m");
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
		value = connect(sock, (struct sockaddr*)&SERVER, sizeof(SERVER));

		if(value != 0)
		{
			printf("\n\n\033[31mError while connecting to %s:%d.\nLet's retry.\033[0m\n\n", buff, port);
			sleep(3);
		}
	}
	
	bool keepGoing = true;
	while(keepGoing)
	{
		memset(buff, 0, BUFFER);

		printf("|: ");
		fgets(buff, BUFFER, stdin);

		send(sock, buff, BUFFER, 0);

		if(!strcmp(buff, END))
		{
			keepGoing = false;
			printf("\n\nLet's close connection with Server...");
		}
	}


	close(sock);
	printf("\n\033[35mConnection successfully closed.\033[35m\n\n");

	return 1;
}
