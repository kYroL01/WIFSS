#include "server.h"


void start_server(void)
{
	clear_console();

	printf("\n\033[32m[WIFSS] Starting Server...\033[0m\n");

	init_global_variables();

	int8_t listen_socket = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if(listen_socket == -1)
	{
		printf("\n\n\033[31m[WIFSS] Error during creation of an endpoint for listening socket: %s.\033[0m\n\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(setsockopt(listen_socket, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) == -1)
	{
		printf("\n\n\033[31m[WIFSS] Error while authorizing the socket to reuse a busy port: %s.\033[0m\n\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in6 server;
	memset(&server, 0, sizeof(server));
	server.sin6_family = AF_INET6;
	server.sin6_addr   = in6addr_any;

	char buffer[BUFFER];
	char *args[BUFFER];
	int16_t nbArgs = -1;

	int8_t res;
	uint32_t port;
	do
	{
		do
		{
			printf("\n-> Listening Port: ");
			prompt_keyboard(buffer);
			free_args(args, &nbArgs);
			parse_command(buffer, args, &nbArgs);

			if(nbArgs == 1)
			{
				port = strtoul(args[0], NULL, 10);
			}

			else
			{
				port = -1;
			}

		} while((port < 1024 && geteuid() != 0) || port > 65535);

		server.sin6_port = htons(port);

		/* Let's save this value in the global variables structure */
		g_core_variables.server_sock = listen_socket;

		res = bind(listen_socket, (struct sockaddr*)&server, sizeof(server));
		if(res == -1)
		{
			printf("\n\n\033[31m[WIFSS] Error during creation of the listening socket: %s.\033[0m\n\n", strerror(errno));
			printf("\nWould you like to retry on another port ? (YES / no)\n\n");

			if(!prompt_yes_no(buffer, args, &nbArgs))
			{
				close_server();
				exit(EXIT_FAILURE);
			}
		}

		else
		{
			break;
		}

	} while(true);

	if(listen(listen_socket, MAX_CLIENTS) == -1)
	{
		printf("\n\n\033[31m[WIFSS] Error during listening on the socket: %s.\033[0m\n\n", strerror(errno));
		close_server();
		exit(EXIT_FAILURE);
	}

	printf("\n\n\033[35m[WIFSS] Server opened, waiting for clients on port %d...\033[0m\n\n", ntohs(server.sin6_port));
}


void close_server(void)
{
	close_all_connections();

	if(g_core_variables.server_sock >= 0)
	{
		if(close(g_core_variables.server_sock) == -1)
		{
			printf("\n\033[35m[WIFSS] Socket of server couldn't be successfully closed.\033[0m\n\n");
		}
		else
		{
			printf("\n\033[32m[WIFSS] Socket of server successfully closed.\033[0m\n\n");
		}
	}

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		pthread_cancel(g_core_variables.threads[i]);
	}

	// SSL stuffs
	SSL_CTX_free(g_core_variables.ctx);
	EVP_cleanup();

	printf("\033[35m[WIFSS] Server is shutting down for now.\033[0m\n");

	for(uint8_t i = 0; i < 64; i++)
	{
		printf("=");
	}

	printf("\n\n");
}


void init_global_variables(void)
{
	printf("\n\033[32m[WIFSS] Initialization of clients list [%d client slots would be available]...\033[0m\n", MAX_CLIENTS);

	g_core_variables.server_port = -1;
	g_core_variables.server_sock = -1;

	for(uint8_t i = 0; i < MAX_CLIENTS; i++)
	{
		g_core_variables.clients[i].id     = -1;
		g_core_variables.clients[i].sock   = -1;
		g_core_variables.clients[i].status = FREE;
		g_core_variables.clients[i].ssl    = NULL;
	}

	// SSL stuffs
	OpenSSL_add_ssl_algorithms();
	SSL_load_error_strings();

	SSL_CTX *ctx = SSL_CTX_new(TLSv1_2_method());
	if(ctx == NULL)
	{
		printf("\n\n");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	SSL_CTX_set_ecdh_auto(ctx, 1);

	if(SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0 || SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0)
	{
		printf("\n\n");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	if(!SSL_CTX_check_private_key(ctx))
	{
		printf("\n\n");
		fprintf(stderr, "\n\033[31mSSL Error: The certificate does not correspond to the private key.\033[0m\n");
		exit(EXIT_FAILURE);
	}

	g_core_variables.ctx = ctx;
}
