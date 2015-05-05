#include <client.h>

int str_beginwith(const char *w,const char *s)
{
	while(*s) {
		if(*s != *w) {
			return 0;
		}
		w++;
		s++;
	}
	return 1;
}

int serverCommunication(int cmd, int sock, char *path)
{
	char __buff[BUFFER];

	switch(cmd)
	{
		case ENDSIG:
			send(sock, END, strlen(END), false);
			recv(sock, __buff, BUFFER, false);
			if(!strcmp(__buff, "stop"))
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;

		case ASKFILE:
			strcpy(__buff, ISPRESENT); //Formatage du buffer... "ispresent path_file"
			strcat(__buff, " ");
			strcat(__buff, path);

			send(sock, __buff, BUFFER, false);
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

		default:
			printf("Unknown signal.\n");
			return 0;
			break;
	}
}
