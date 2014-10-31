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
	header_t header = {-1,-1,-1}; // Servira de header de réception et d'envoi, il faudra juste changer la taille
	char* message = NULL;
	char* startMsg; // Permet de connaître la première adresse en mémoire du msg pour pouvoir la libérer par la suite
	char* responseForServ = NULL; // La réponse que l'on envoie au serveur
	int initCo = 1; // Quand initCo == 1 il faut juste envoyer un header au serveur pour qu'il donne un id au client
	
	
	linkedlist_t listResp = NULL; // La liste des réponses de commande 
	command_t command = {NULL, NULL};
	
	
	while(1){
		sock = connectCli("127.0.0.1");
		
		
		printf("Connexion serveur OK\n");
		
		if(initCo == 1){ // On veut initialiser la connexion
			if(sendHeader(sock, header) == -1){
				perror("Erreur dans l'envoi de l'entete au serveur");
				exit(EXIT_FAILURE);
			}
			initCo = 0; // La connexion est initialisé, le serveur va envoyer les données
		}
		else if(responseForServ != NULL){
			header.size = strlen(responseForServ)+1;
			if(sendHeader(sock, header) == -1){
				perror("Erreur dans l'envoi de l'entete au serveur");
				exit(EXIT_FAILURE);
			}
			sendMessage(sock, "%s", responseForServ);
			free(responseForServ);
		}
		// Dans les ca où il n'y a pas de réponse à envoyer, on attend des données du serveur
		
		
		if(recvHeader(sock, &header) == -1){
			perror("Erreur dans la réception de l'entete");
			exit(EXIT_FAILURE);
		}
		
		message = recvMessage(sock, header); // Reception du message
		if(message == NULL){
			perror("Erreur dans la reception du message");
			exit(EXIT_FAILURE);
		}
		if(shutdown(sock, SHUT_RDWR) == -1){
			perror("Erreur pendant la fermeture du socket.");
			exit(EXIT_FAILURE);
		}
		close(sock);
		
		
		startMsg = message;
		do{ // Récupère et interprète toutes les commandes
			command = getCommand(&message);
			listResp = performCommandCli(&command, listResp);
			command.args = cleanL(command.args, 0); // Vide la liste d'arguments s'il en reste
		}while(command.name != NULL);
		
		responseForServ = formatResponse(&listResp);
		free(startMsg);
	}
	
	return EXIT_SUCCESS;
}


