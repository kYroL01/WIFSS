#ifndef __CLIENT__
#define __CLIENT__


#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <ctype.h>
#include <pthread.h>


#define BUFFER 256

#define true  1
#define false 0
typedef char bool;


#include <com.h>
#include <str.h>


//DATA_MUTEXED
typedef struct
{
	int sock;
	bool keepGoing;
	bool tunnelOpened;

	pthread_mutex_t mutex;

} MUTEX;


//START_STOP
bool init(struct sockaddr_in*, int*);
void disconnect(int);

//COMMUNICATION
void handle_command(const char*, MUTEX*);
void communication(MUTEX*);

//TRANSFER
int upload(const char*, int);
int download(const char*, int);

//TUNNEL
void startunnel(MUTEX*, int);
void acceptunnel(MUTEX*, int);

//THREAD_COMMUNICATION
void* serverCommunication(void*);
void* clientCommunication(void*);


#endif
