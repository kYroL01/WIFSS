#include <server.h>

int isvalid(const char *s)
{
	if(str_beginwith(s, QUIT))
	{
		return 0;		
	}

	return 1;
}

void broadcast(int sender, const char *msg)
{
	int i;
	
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if(i != sender)
		{
			send(g_clients[i].sock, msg, BSIZE, 0);
		}
	}
}

void close_all_connections()
{
	int i;

	printf("[WIFSS] Closing all connections...");
	
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		close(g_clients[i].sock);
	}

	printf("[WIFFS] ... done.\n");
}

int process_command(const char *command, int sender_id)
{
	char cpy[BSIZE] = ""; // Pour copier le message 
	char buffer[BSIZE] = "";
	int fsize;

	if(str_beginwith(command, DOWNLOAD))
	{
		int remote_id = -1;
		char filename[BSIZE] = "";
	
		printf("[Client %d] Asking for upload !\n", sender_id);
		
		sscanf(command, "download %s %d", filename, &remote_id);
		printf("[Client %d] File: \"%s\" of %d\n", sender_id, filename, remote_id);
		
		if((remote_id > (MAX_CLIENTS - 1)) || (remote_id < 0) || (remote_id == sender_id)) {
			send(g_clients[sender_id].sock, "[WIFFS] Error: Client wanted is invalid...\n",
										 		52, 0);
			return 0;
		}
			
		
		if(g_clients[remote_id].sock <= 0) {
			send(g_clients[sender_id].sock, "[WIFFS] Error: Client asked is not connected...\n",
										 		52, 0);
			return 0;
		}
		
		sprintf(cpy, "upload %s", filename);
		send(g_clients[remote_id].sock, cpy, BSIZE, 0);
		
		recv(g_clients[remote_id].sock, buffer, BSIZE, 0);
		sscanf(buffer, "size: %d", &fsize);
		printf("[WIFFS] File size: %d.\n", fsize);
		
		while(strcmp(buffer, ENDT)){
			recv(g_clients[remote_id].sock, buffer, BSIZE, 0);
			printf("%s", buffer);
		}	
		printf("\n");
		
		
		return 0;
	}
	
	else if(str_beginwith(command, QUIT)) {
		close(g_clients[sender_id].sock);
		return 1;
	}
	else {
		memset(cpy, 0, BSIZE);
		sprintf(cpy, "[Client %d] : %s", sender_id, command);
		broadcast(sender_id,cpy);
	}
	return 1;
}

void* on_connection(void *data)
{
	char buffer[BSIZE] = "";
	
	int res;
	struct client_t client = *((struct client_t*)data);

	printf("[WIFSS] Connection received of %s\n", inet_ntoa(client.addr.sin_addr));

	sprintf(buffer, "[Client %d] Connecting...\n", client.id);
	broadcast(client.id, buffer);
	
	while(client.sock > 0)
	{
		memset(buffer, 0, BSIZE);
		res = recv(client.sock, buffer, BSIZE, 0);
		if(res <= 0) {
			break; // Le client s'est déco
		}

		printf("[Client %d] Received: %s", client.id, buffer);
		
		process_command(buffer, client.id);
	}

	printf("[Client %d] Deconnection...\n", client.id);
	sprintf(buffer, "(Client %d is deconnecting.)\n", client.id);
	broadcast(client.id, buffer);
	close(client.sock);
	
	g_clients[client.id].status = FREE; 
	return NULL;
}

int start_server(void)
{
	int res = 0, i;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int listen_socket, sock;
	unsigned int current_id = 0, asize;
	

	server.sin_port 		= htons(PORT);
	server.sin_family 		= AF_INET;
	server.sin_addr.s_addr 	= INADDR_ANY;

	listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	res = bind(listen_socket, (struct sockaddr*)&server, sizeof(server));
	if(res < 0) {
		printf("Error bind (%d)\n", res);
		return res;
	}
	listen(listen_socket, MAX_CLIENTS);

	
	printf("\n[WIFSS] Initialisation of Clients list..\n");
	for(i=0;i<MAX_CLIENTS;i++) {
		g_clients[i].status = FREE;
	}
	
	asize = sizeof(struct sockaddr_in);
	
	printf("[WIFSS] Server opened, waiting for Clients...\n");
	
	
	pthread_create(&command_thread, NULL, &command_handler, NULL);
	while((sock = accept(listen_socket, (struct sockaddr*)&client, &asize)) && (count < MAX_CLIENTS) ) {
		struct client_t new_client;

		for(i=0;i<MAX_CLIENTS;i++)	{
			if(g_clients[i].status == FREE) {// Emplacement libre
				current_id = i;
				g_clients[i].status = TAKEN;
				break;
			}
		}
		
		printf("[WIFSS] Connection received %s:%hu -> Number given: %hd.\n", inet_ntoa(client.sin_addr), (unsigned short int)ntohs(client.sin_port),
																   current_id);
		
		new_client.id 		= current_id;
		new_client.addr 	= client;
		new_client.sock 	= sock;
		new_client.status	= TAKEN;

		g_clients[current_id] = new_client; // On le copie dans la structure globale
		res = pthread_create(&threads[current_id], NULL, &on_connection, (void*)&new_client);
		
		if(res != 0) {
			printf("[WIFFS] Error during Thread creation %d: Error %d.\n", current_id, res);
			broadcast(SID, "Server fatal error, stopping now.\n");
			close_all_connections();
			return res;
		}
	
		/* Calcule le nombre de clients connectés */
		count = 0;
		for(i=0;i<MAX_CLIENTS;i++) {
			if(g_clients[i].status == TAKEN) {
				count++;
			}
		}
		printf("[WIFSS] There is %d client(s) connected.\n", count);
	}
	
	if(count >= MAX_CLIENTS)
		broadcast(SID, "Maximum capacity reached, stopping now.\n");
	
	close_all_connections();
	close(sock);
	close(listen_socket);

	return 0;
}

int main()
{
	start_server();
	return 0;
}

