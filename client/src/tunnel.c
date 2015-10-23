#include <client.h>

void startunnel(MUTEX *data, int idClient)
{
	char _buff[BUFFER] = {0};

	printf("\n[WIFFS] Let's create a tunnel with %d Client...", idClient);
	sprintf(_buff, "%s %d", ASKTUNNEL, idClient);
	send(data->sock, _buff, BUFFER, false);
	memset(_buff, 0, BUFFER);
	recv(data->sock, _buff, BUFFER, false);

	if(!strcmp(_buff, OKFORTUN))
	{
		data->tunnelOpened = true;
	}
	else
	{
		data->tunnelOpened = false;
		printf("[WIFFS] Can't open a tunnel with %d Client.\n", idClient);
	}
}

void acceptunnel(MUTEX *data, int clientAsking)
{
	char _buff[BUFFER] = {0};

	printf("Client %d is asking you to start a tunnel. Accept ? (Yes / No)\n\n:| ", clientAsking);

	promptKeyboard(_buff);

	if(!strcmp(_buff, "yes") || !strcmp(_buff, "y"))
	{
		memset(_buff, 0, BUFFER);
		sprintf(_buff, "%s", OKFORTUN);
		send(data->sock, _buff, BUFFER, false);

		data->tunnelOpened = true;
	}
	else
	{
		memset(_buff, 0, BUFFER);
		sprintf(_buff, "%s", KOFORTUN);
		send(data->sock, _buff, BUFFER, false);
		data->tunnelOpened = false;
	}
}
