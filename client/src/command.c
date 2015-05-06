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


int s_Com(int cmd, int sock, const char *path = NULL, int idClient = 0)
{
	char __buff[BUFFER];
	memset(__buff, 0, BUFFER);

	switch(cmd)
	{
		case ENDSIG:
			send(sock, END, strlen(END), false);
			memset(__buff, 0, BUFFER);
			recv(sock, __buff, BUFFER, false);
			if(!strcmp(__buff, STOP))
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		case ASKFILE:
			sprintf(__buff, "%s %s %d", ISPRESENT, path, id_Client);
			send(sock, __buff, BUFFER, false);
			memset(__buff, 0, BUFFER);
			sleep(1);
			recv(sock, __buff, BUFFER, false);
			if(!strcmp(__buff, PRESENT))
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		case FILEDWL:
			send(sock, FINISHED, strlen(FINISHED), false);
			memset(__buff, 0, BUFFER);
			recv(sock, __buff, BUFFER, false);
			if(!strcmp(__buff, OK))
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		case ASKLIST:
			send(sock, LIST, strlen(LIST), false);
			memset(__buff, 0, BUFFER);
			recv(sock, __buff, BUFFER, false);
			if(str_beginwith(__buff, "list:")) //Le Server commence la phrase par "list:"
			{
				puts(__buff);
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		default:
			printf("Unknown signal.\n");
			return 0;
			break;
	}
}
