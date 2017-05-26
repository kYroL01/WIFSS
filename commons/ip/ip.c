#include "ip.h"


void printEndpoint(const struct addrinfo *const p)
{
	void *addr = NULL;
	uint16_t port = -1;

	if(p->ai_family == AF_INET)
	{
		addr = &(((struct sockaddr_in*)p->ai_addr)->sin_addr);
		port = ((struct sockaddr_in*)p->ai_addr)->sin_port;
	}

	else
	{
		addr = &(((struct sockaddr_in6*)p->ai_addr)->sin6_addr);
		port = ((struct sockaddr_in6*)p->ai_addr)->sin6_port;
	}

	char ipstr[INET6_ADDRSTRLEN];
	if(inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr) == NULL)
	{
		printf("\n\033[31m[WIFSS] Error while converting an IP address to string: %s.\033[0m\n\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("\'%s [%d]\'", ipstr, ntohs(port));
}
