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
#include <signal.h>


#define BUFFER    512
#define FGETSBUFF 256
#define PATHSIZE   64
#define TALLERCMD  32

#define true  1
#define false 0


#include <com.h>
#include <str.h>


//DATA_MUTEXED
typedef struct
{
	int sock;
	_Bool keepGoing;

	pthread_mutex_t mutex;

} MUTEX;


//START / STOP
_Bool init(struct sockaddr_in*, int*);
void disconnect(int);

//COMMUNICATION
void handle_command(const char*, MUTEX*);

//TRANSFER
void upload(const char*, int);
void download(const char*, int);
void closeFile(FILE*, const char*);

//THREADS
void* serverCommunication(void*);
void* clientCommunication(void*);
void* infiniteWaitingFnct(void*);


#endif
