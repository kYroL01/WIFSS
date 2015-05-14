#include <client.h>

void startunnel(int sock, int idClient)
{
	char _buff[BUFFER] = {0};

	printf("\nLet's create a tunnel with %d Client...", idClient);

	sprintf(_buff, "%s %d", ASKTUNNEL, idClient);

	send(sock, _buff, BUFFER, false);
	
	memset(_buff, 0, BUFFER);

	recv(sock, _buff, BUFFER, false);


	if(!strcmp(_buff, OKFORTUN))
	{
		_tunnelOpened_ = true;
		while(_tunnelOpened_)
		{
			communication(sock, &_tunnelOpened_, true);
		}
	}
	else
	{
		_tunnelOpened_ = false;
		printf("Can't open a tunnel with %d Client.\n", idClient);
	}
}

void acceptunnel(int sock, int clientAsking)
{
	char _buff[BUFFER] = {0};

	printf("Client %d is asking you to start a tunnel. Accept ? (Yes / No)\n\n:| ", clientAsking);

	scanf("%s", _buff);

	str_lowerCase(_buff);

	if(!strcmp(_buff, "yes") || !strcmp(_buff, "y"))
	{
		memset(_buff, 0, BUFFER);
		sprintf(_buff, "%s", OKFORTUN);
		send(sock, _buff, BUFFER, false);

		_tunnelOpened_ = true;
		while(_tunnelOpened_)
		{
			communication(sock, &_tunnelOpened_, true);
		}
	}
	else
	{
		memset(_buff, 0, BUFFER);
		sprintf(_buff, "%s", KOFORTUN);
		send(sock, _buff, BUFFER, false);
		_tunnelOpened_ = false;
	}
}
