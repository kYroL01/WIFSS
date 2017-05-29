#include "client.h"


bool start_client(void)
{
	clear_console();

	printf("\n\033[32m[WIFSS] Starting Client...\033[0m\n\n");

	init_global_variables();

	char address[BUFFER];
	char *args[BUFFER];
	int16_t nbArgs = -1;

	do
	{
		printf("-> Server IP: ");
		prompt_keyboard(address);
		free_args(args, &nbArgs);
		parse_command(address, args, &nbArgs);

	} while(nbArgs != 1);

	int32_t port;
	char portBuffer[BUFFER];
	do
	{
		printf("-> Server Port: ");
		prompt_keyboard(portBuffer);
		free_args(args, &nbArgs);
		parse_command(portBuffer, args, &nbArgs);

		if(nbArgs == 1)
		{
			port = strtoul(args[0], NULL, 10);
		}

		else
		{
			port = -1;
		}

	} while(port < 0 || port > 65535);

	struct addrinfo *servinfo;

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	int16_t status = getaddrinfo(address, args[0], &hints, &servinfo);
	if(status != 0)
	{
	    fprintf(stderr, "\n\033[31m[WIFSS] An error occurred while trying to get some information about your host: %s.\033[0m\n\n", gai_strerror(status));
	    exit(EXIT_FAILURE);
	}

	char buffer[BUFFER];

	// Some stuff to set a timeout on `connect`
	int16_t sock     = -1;
	int8_t error     = 0;
	socklen_t lenght = sizeof(error);
	int16_t nbFds    = -1;
	fd_set readfds, writefds;
	do
	{
		for(struct addrinfo *tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
		{
			/* Only for previous iterations */
			if(sock != -1 && close(sock) == -1)
			{
				fprintf(stderr, "\n\033[31m[WIFSS] An error occurred while closing a socket: %s.\033[0m\n\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			/* ____________________________ */

			sock = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
			if(sock == -1)
			{
				printf("\n\033[31m[WIFSS] Error while creating an endpoint for communication with server: %s.\033[0m\n\n", strerror(errno));
				return false;
			}

			// Before the connection procedure, we'll set the socket as NON-BLOCKING
			fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, false) | O_NONBLOCK);

			// Let's launch the connection procedure
			connect(sock, tmp->ai_addr, tmp->ai_addrlen);
			if(errno != EINPROGRESS)
			{
				fprintf(stderr, "\n\033[31m[WIFSS] An error occurred while running the connection procedure to: ");
				printEndpoint(tmp);
				printf(" (%s).\033[0m\n\n", strerror(errno));
				continue;
			}

			// Let's now set a watch dog of 3 seconds on it
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_SET(sock, &readfds);
			nbFds = 1; /* select(sock + 1, &readfds, &writefds, NULL, &(struct timeval){3, 0}); */ /* THIS IS TEMPORARY ! */
			if(nbFds == 0)
			{
				// The timeout has been elapsed...
				printf("\n\033[31m[WIFSS] Error while connecting to ");
				printEndpoint(tmp);
				printf(": timeout reached.\033[0m\n");
				continue;
			}

			else if(nbFds == -1)
			{
				// Again ? An error occurred...
				fprintf(stderr, "\n\033[31m[WIFSS] An error occurred while waiting for the connection procedure ending: %s.\033[0m\n\n", strerror(errno));
				exit(EXIT_FAILURE);
			}

			// Two cases: The connection has been established OR a f*cking new error occurred (before the timeout !)...
			if(getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &lenght) != 0)
			{
				fprintf(stderr, "\n\033[31m[WIFSS] An error occurred while retrieving some information about the socket: %s.\033[0m\n\n", gai_strerror(status));
				exit(EXIT_FAILURE);
			}

			if(error == 0)
			{
				// For the future, let's set again the socket as BLOCKING
				fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, false) ^ O_NONBLOCK);

				// SSL stuffs
				SSL *ssl = SSL_new(g_core_variables.ctx);
				if(ssl == NULL)
				{
					fprintf(stderr, "\n\n\033[31mSSL Error: Couldn\'t enable the SSL layer on the socket.\n\n");
					exit(EXIT_FAILURE);
				}

				if(SSL_set_cipher_list(ssl, (const char *const)"HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4") != 1)
				{
					fprintf(stderr, "\n\n\033[31mSSL Error: Couldn\'t set the ciphers list.\n\n");
					exit(EXIT_FAILURE);
				}

				SSL_set_fd(ssl, sock);

				if(SSL_connect(ssl) <= 0)
				{
					printf("\n\n\033[31m[WIFSS] Couldn\'t create a TLS tunnel with the host...\033[0m\n\n");
					ERR_print_errors_fp(stderr);
					exit(EXIT_FAILURE);
				}

				// SSL Verification + Certificate information
				if(SSL_get_verify_result(ssl) != X509_V_OK)
				{
					fprintf(stderr, "\n\n\033[31mSSL Error: The result got from SSL is not valid.\n\n");
					exit(EXIT_FAILURE);
				}

				X509 *cert = SSL_get_peer_certificate(ssl);
				if(cert == NULL)
				{
					fprintf(stderr, "\n\n\033[31mSSL Error: No certificate was sent by server.\n\n");
					exit(EXIT_FAILURE);
				}

				printf("\n\033[34m\tTLS cipher used for this connection: %s\n", SSL_get_cipher(ssl));

				char *str = NULL;
				printf("\tServer certificate:\n");
				str = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
				printf("\t\tSubject: %s\n", (str != NULL ? str : "None"));
				OPENSSL_free(str);
				str = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
				printf("\t\tIssuer : %s\033[0m\n", (str != NULL ? str : "None"));
				OPENSSL_free(str);
				X509_free(cert);
				// __________________________________________

				g_core_variables.ssl = ssl;

				printf("\n\033[32m[WIFSS] Connected to ");
				printEndpoint(tmp);
				printf(".\033[0m\n");

				strncpy(buffer, "", BUFFER);
				SSL_read(ssl, buffer, BUFFER);
				uint8_t temp_id = getSecondArgsGroupAsInteger(buffer);
				printf("\n\033[32m[WIFSS] Your id on the server is %d.\033[0m\n\n", temp_id);

				/* We save the ID of the client for the future */
				g_core_variables.client_id = temp_id;

				break;
			}

			else
			{
				printf("\n\033[31m[WIFSS] Error while connecting to ");
				printEndpoint(tmp);
				printf(": %s.\033[0m\n", strerror(errno));
			}
		}

		if(g_core_variables.client_id != -1)
		{
			// Yeah... We've to `break` the loop a second time 
			break;
		}

		else
		{
			printf("\nWould you like to retry now ? (YES / no)\n\n");
			if(!prompt_yes_no(buffer, args, &nbArgs))
			{
				printf("\n");
				return false;
			}
		}

	} while(true);

	// We free here the previous linked list
	freeaddrinfo(servinfo);

	/* We save the socket value for the future */
	g_core_variables.server_sock = sock;

	return true;
}


void stop_client(void)
{
	SSL_shutdown(g_core_variables.ssl);
	SSL_free(g_core_variables.ssl);

	if(g_core_variables.server_sock >= 0)
	{
		if(close(g_core_variables.server_sock) == -1)
		{
			printf("\n\033[35m[WIFSS] Socket couldn't be successfully closed.\033[0m\n\n");
		}
		else
		{
			printf("\n\n[WIFSS] Socket successfully closed.\n\n");
		}
	}

	SSL_CTX_free(g_core_variables.ctx);
	
	printf("[WIFSS] Client is shutting down for now.\n");

	for(uint8_t i = 0; i < 64; i++)
	{
		printf("=");
	}

	printf("\n\n");
}


void init_global_variables(void)
{
	g_core_variables.server_sock = -1;
	g_core_variables.client_id   = -1;
	g_core_variables.ssl         = NULL;

	// SSL stuffs
	OpenSSL_add_ssl_algorithms();
	SSL_load_error_strings();

	SSL_CTX *ctx = SSL_CTX_new(TLSv1_2_client_method());
	if(ctx == NULL)
	{
		fprintf(stderr, "\n\033[31m\033[31mSSL Error: Couldn\'t create a SSL context.\033[0m\n\n");
		exit(EXIT_FAILURE);
	}

	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION);

	if(SSL_CTX_load_verify_locations(ctx, "client.crt", NULL) != 1)
	{
		fprintf(stderr, "\n\033[31mSSL Error: Couldn\'t verify the location of, or load, the server\'s certificate.\033[0m\n\n");
	}

	g_core_variables.ctx = ctx;
}
