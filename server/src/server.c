#include <server.h>

int isvalid(const char *s)
{
	// pour se déconnecter un client envoie une chaine commencant par "qxt"
	if(str_beginwith(s, "quit"))
		return 0;
	return 1;
}

void broadcast(int sender, const char *msg)
{
	int i;
	
	for(i=0;i<MAX_CLIENTS;i++) {
		if(i != sender) {
			send(g_clients[i].sock, msg, BSIZE, 0);
		}
	}
}

void close_all_connections()
{
	int i;
	printf("[WIFSS] Fermeture de toutes les connections.... ");
	
	for(i=0;i<MAX_CLIENTS;i++)
		close(g_clients[i].sock);
	printf("fait.\n");
}

int process_command(const char* command, int sender_id)
{
	char cpy[BSIZE] = ""; // Pour copier le message 
	char buffer[BSIZE] = "";
	int fsize;

	if(str_beginwith(command, "download"))	{
		int remote_id = -1;
		char filename[BSIZE] = "";
	
		printf("[Client %d] Demande de téléchargement !\n", sender_id);
		
		sscanf(command, "download %s %d", filename, &remote_id);
		printf("[Client %d] Fichier : \"%s\" de %d\n", sender_id, filename, remote_id);
		
		if((remote_id > (MAX_CLIENTS - 1)) || (remote_id < 0) || (remote_id == sender_id)) {
			send(g_clients[sender_id].sock, "Erreur : Le client demandé est incorrect..\n",
										 		52, 0);
			return 0;
		}
			
		
		if(g_clients[remote_id].sock <= 0) {
			send(g_clients[sender_id].sock, "Erreur : Le client demandé n'est pas connecté..\n",
										 		52, 0);
			return 0;
		}
		
		sprintf(cpy, "upload %s", filename);
		send(g_clients[remote_id].sock, cpy, BSIZE, 0);
		
		recv(g_clients[remote_id].sock, buffer, BSIZE, 0);
		sscanf(buffer, "size: %d", &fsize);
		printf("Taille du fichier : %d\n", fsize);
		
		while(strcmp(buffer, "ENDT")) {
			recv(g_clients[remote_id].sock, buffer, BSIZE, 0);
			printf("%s", buffer);
		}	
		printf("\n");
		
		
		return 0;
	}
	
	else if(str_beginwith(command, "quit")) {
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

	printf("[WIFSS] Connexion reçue de %s\n", inet_ntoa(client.addr.sin_addr));

	sprintf(buffer, "[Client %d] Se connecte.\n", client.id);
	broadcast(client.id, buffer);
	
	while(client.sock > 0)
	{
		memset(buffer, 0, BSIZE);
		res = recv(client.sock, buffer, BSIZE, 0);
		if(res <= 0) {
			break; // Le client s'est déco
		}

		printf("[Client %d] Reçu : %s", client.id, buffer);
		
		process_command(buffer, client.id);
	}

	printf("[Client %d] Déconnexion..\n", client.id);
	sprintf(buffer, "(Le client %d se déconnecte.)\n", client.id);
	broadcast(client.id, buffer);;
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
		printf("Erreur bind (%d)\n", res);
		return res;
	}
	listen(listen_socket, MAX_CLIENTS);

	
	printf("[WIFSS] Initialisation de la liste des clients..\n");
	for(i=0;i<MAX_CLIENTS;i++) {
		g_clients[i].status = FREE;
	}
	
	asize = sizeof(struct sockaddr_in);
	
	printf("[WIFSS] Serveur allumé, attente de client..\n");
	
	
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
		
		printf("[WIFSS] Connexion reçue  %s:%d -> numéro attribué %d \n", inet_ntoa(client.sin_addr), (unsigned int)ntohs(client.sin_port),
																   current_id);
		
		new_client.id 		= current_id;
		new_client.addr 	= client;
		new_client.sock 	= sock;
		new_client.status	= TAKEN;

		g_clients[current_id] = new_client; // On le copie dans la structure globale
		res = pthread_create(&threads[current_id], NULL, &on_connection, (void*)&new_client);
		
		if(res != 0) {
			printf("Erreur lors de la création du thread %d : Erreur %d \n", current_id, res);
			broadcast(SID, "Erreur critique du serveur, arrêt.\n");
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
		printf("[WIFSS] Il y a %d client(s) connecté(s).\n", count);
	}
	
	if(count >= MAX_CLIENTS)
		broadcast(SID, "Capacitié dépassée, arrêt du serveur.\n");
	
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

