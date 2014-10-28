#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>



#include "../core.h"
#include "../parser.h"
#include "../linkedlist.h"
#include "../interpreter.h"

/* Main coté Client */
int main(){
	SOCKET sock;
	struct sockaddr_in sin = {0};
	int sockOptions = 1;
	char* format = "-> MSG : %s \n-> Id : %d";
	header_t header;
	char* message = NULL;
	char* startMsg; // Permet de connaître la première adresse en mémoire du msg pour pouvoir la libérer par la suite
	
	linkedlist_t listResp = NULL; // La liste des réponses de commande 
	command_t command = {NULL, NULL};
	
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
			perror("Error socket");
			exit(EXIT_FAILURE);
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	// Permet de ne pas avoir d'erreur de type : endpoint is not connected
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockOptions, sizeof(sockOptions)) == -1){
		perror("Error setsockopt");
		exit(EXIT_FAILURE);
	}
	
	
	if(connect(sock, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("Error server connection");
		exit(EXIT_FAILURE);
	}
	
	printf("Connexion serveur OK\n");
	
	
	
	sendMessage(sock, format, "Bonjour je suis un client", 10);
	
	if(recvHeader(sock, &header) == -1){
			perror("Error recv header");
			exit(EXIT_FAILURE);
	}
	
	message = recvMessage(sock, header); // Reception du message
	if(message == NULL){
		perror("Erreur dans la reception du message");
		exit(EXIT_FAILURE);
	}
	startMsg = message;
	
	if(shutdown(sock, SHUT_RDWR) == -1){
		perror("Erreur pendant la fermeture du socket.");
		exit(EXIT_FAILURE);
	}
	close(sock);
	
	
	do{
		command = getCommand(&message);
		listResp = performCommandCli(&command, listResp);
		command.args = cleanL(command.args); // Vide la liste d'arguments s'il en reste
	}while(command.name != NULL);
	element* el;
	el = popL(&listResp);
	printf("El->type : %d et INT : %d et el->val: %d\n", INT, el->type, (int)el->val);
	
	free(startMsg);
	
	
	return EXIT_SUCCESS;
}


