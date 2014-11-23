#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#include "coreCli.h"
#include "../core.h"
#include "../parser.h"
#include "../linkedlist.h"
#include "../interpreter.h"


/* /////////////////////////////////////////////////////////////////////////// 
	TODO :
		
/////////////////////////////////////////////////////////////////////////// */




/* Main coté Client */
int main(){
	SOCKET sock;
	header_t header = {-1,-1,-1}; // Servira de header de réception et d'envoi, il faudra juste changer la taille
	char* message = NULL;
	char* startMsg; // Permet de connaître la première adresse en mémoire du msg pour pouvoir la libérer par la suite
	int initCo = 1; // Quand initCo == 1 il faut juste envoyer un header au serveur pour qu'il donne un id au client
	char* responseForServ = NULL;
	
	linkedlist_t listResp = NULL; // La liste des réponses de commande 
	command_t command = {NULL, NULL};
	
	int idGameBefore;
	
	while(1){
		sock = connectCli("127.0.0.1");
		
		
		// On veut initialiser la connexion
		if(initCo == 1){ 
			if(sendHeader(sock, header) == -1){
				perror("Erreur dans l'envoi de l'entete au serveur");
				exit(EXIT_FAILURE);
			}
			printf("Vous etes connecte au serveur.\n");
			initCo = 0; // La connexion est initialisé, le serveur va envoyer les données
			sleep(1);
		}
		else if(responseForServ != NULL){
			header.size = strlen(responseForServ)+1;
			if(sendHeader(sock, header) == -1){
				perror("Erreur dans l'envoi de l'entete au serveur");
				exit(EXIT_FAILURE);
			}
			sendMessage(sock, responseForServ);
			free(responseForServ);
		}
		else{ // Aucune réponse à envoyer au serveur
			header.size = -1;
			if(sendHeader(sock, header) == -1){
				perror("Erreur dans l'envoi de l'entete au serveur");
				exit(EXIT_FAILURE);
			}
		}
		// Dans les ca où il n'y a pas de réponse à envoyer, on attend des données du serveur
		
		idGameBefore = header.idGame;
		if(recvHeader(sock, &header) == -1){
			perror("Erreur dans la réception de l'entete");
			exit(EXIT_FAILURE);
		}
		// Quand le serveur a envoyé un messsage
		if(header.size != -1){
			message = NULL;
			message = recvMessage(sock, header); // Reception du message
			if(message == NULL){
				perror("Erreur dans la reception du message.");
				exit(EXIT_FAILURE);
			}
			
			
			startMsg = message;
			do{ // Récupère et interprète toutes les commandes
				command = getCommand(&message);
				listResp = performCommandCli(&command, listResp);
				command.args = cleanL(command.args, 0); // Vide la liste d'arguments s'il en reste
			}while(command.name != NULL);
			fflush(stdout);
			
			responseForServ = formatResponse(&listResp);
			free(startMsg);
			if(responseForServ == NULL){
				sleep(1); // Quand on a pas de réponse, on attend 1 secondes
			}
		}
		
		// On est rentré dans une partie, on peut maintenant dialoguer avec le serveur sans fermer les sockets
		if(idGameBefore != header.idGame && header.idGame >= 0){
			connectedMode(sock); // Commence une partie en mode connecté
			header.idGame = -1;
		}
		
		
		if(shutdown(sock, SHUT_RDWR) == -1){
			perror("Erreur pendant la fermeture du socket.");
			exit(EXIT_FAILURE);
		}
		close(sock);
	}
	
	return EXIT_SUCCESS;
}


