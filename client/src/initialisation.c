#include <client.h>



void initialisation(struct sockaddr_in *SERVER, short int *sock)
{
	char __buff[BUFFER];
	short int __port;

	printf("\n\033[32mStarting client...\033[0m\n");

	printf("\nServer IP: ");
	fgets(__buff, BUFFER, stdin);

	do
	{
		printf("\nPort: ");
		scanf("%hd", &__port);

	} while(__port > 65535 || __port < 1024);

	SERVER->sin_family      = AF_INET;
	SERVER->sin_port        = htons(__port);
	SERVER->sin_addr.s_addr = inet_addr(__buff);

	*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	short int result;
	do
	{
		result = connect(*sock, (struct sockaddr*)SERVER, sizeof(*SERVER));
		printf("\n\n\033[31mError while connecting to %s:%hd.\nLet's retry.\033[0m\n\n", inet_ntoa(SERVER->sin_addr.s_addr), port);
		sleep(3);

	} while(result == -1);

	printf("\nConnected to %s:%hd.\n\n", inet_ntoa(SERVER.sin_addr.s_addr), port);
}
