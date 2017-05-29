#ifndef __WIFFS__
#define __WIFFS__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>

#if SSLEAY_VERSION_NUMBER >= 0x0907000L
	#include <openssl/conf.h>
#endif

#define BUFFER        1024
#define COMMANDBUFFER 2048


#endif
