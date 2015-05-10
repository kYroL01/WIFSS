#include <client.h>

void tunneling(int sockServ, int idClient)
{
	char _buff[BUFFER] = {0};

	printf("\nLet's create a tunnel with %d Client...\nGathering client data from server...", idClient);

	sprintf(_buff, "%s %d", DATACLIENT, idClient);

	send(sockServ, _buff, BUFFER, false); //On demande les DATA du Client correspondant au Server
	
	memset(_buff, 0, BUFFER);

	recv(sockServ, _buff, BUFFER, false);

	if(!str_beginwith(_buff, IDCLIENT))
	{
		printf("\n\033[31mCan't access to client data...\033[0m\n");
	}

	else
	{
		short int _port;
		char _addr[16] = {0};
		struct sockaddr_in _clientTunneled;

		sscanf(_buff, "idclient: %s %hd", _addr, &_port);

		_clientTunneled.sin_family      = AF_INET;
		_clientTunneled.sin_port        = htons(_port);
		_clientTunneled.sin_addr.s_addr = inet_addr(_addr);

		int _sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		int _result = connect(_sockClient, (struct sockaddr*)&_clientTunneled, sizeof(_clientTunneled));
	
		if(_result <= 0)
		{
			printf("\n\033[31mCan't connect to %d Client.\033[0m\n\n", idClient);
		}

		else
		{
			bool keepGoing = true;
			while(keepGoing)
			{
				memset(_buff, 0, BUFFER);
				recv(_sockClient, _buff, BUFFER, false);
				memset(_buff, 0, BUFFER);
				send(_sockClient, _buff, BUFFER, false);

				handle_command(_buff, _sockClient, NULL);
			}
		}
	}
}
