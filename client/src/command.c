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


int s_Com(int cmd, int sock, const char *path, int idClient)
{
	char _buff[BUFFER];
	memset(_buff, 0, BUFFER);

	switch(cmd)
	{
		case ENDSIG:
			send(sock, END, strlen(END), false);
			memset(_buff, 0, BUFFER);
			recv(sock, _buff, BUFFER, false);
			if(!strcmp(_buff, STOP))
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		case ASKFILE:
			sprintf(_buff, "%s %s %d", ISPRESENT, path, idClient); // "ispresent path/ client"
			send(sock, _buff, BUFFER, false);
			memset(_buff, 0, BUFFER);
			sleep(1);
			recv(sock, _buff, BUFFER, false);
			if(!strcmp(_buff, PRESENT))
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
			memset(_buff, 0, BUFFER);
			recv(sock, _buff, BUFFER, false);
			if(!strcmp(_buff, OK))
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
			memset(_buff, 0, BUFFER);
			recv(sock, _buff, BUFFER, false);
			if(str_beginwith(_buff, "list:")) //Le Server commence la phrase par "list:"
			{
				puts(_buff);
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		default:
			printf("Unknown signal (\'commands\' Switch).\n");
			return 0;
			break;
	}
}
