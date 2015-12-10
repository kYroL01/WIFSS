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
#include <dirent.h>
#include <sys/stat.h>


#define BUFFER    512
#define FGETSBUFF 256
#define PATHSIZE   64
#define TALLERCMD  32
#define MAXFILEDIR 16

#define true  1
#define false 0


#define PATHWORKINGDIR "/Downloads/WIFSS/"


#include <com.h>
#include <str.h>


/* DATA_MUTEXED */
typedef struct
{
	int sock;
	_Bool keepGoing;

	pthread_mutex_t mutex;

} DATA;


/* START / STOP */
_Bool start(struct sockaddr_in*, int*);
void stop(int);

/* COMMUNICATION */
void handle_command(const char*, DATA*);

/* TRANSFER */
void upload(const char*, int);
void download(const char*, int);
void closeFile(FILE*, const char*);

/* FILE MANAGEMENT */
_Bool setWorkDir(void);
void listFiles(char*);
void removeFile(const char*);
void renameFile(const char*);
void isPresent(const char*, int);
void asklist(const char*, int);
_Bool checkDownloadFolder();

/* THREADS */
void* serverCommunication(void*);
void* clientCommunication(void*);
void* infiniteWaitingFnct(void*);


#endif
