#include <stdio.h>
#include <stdlib.h>

#include "coreCli.h"
#include "../core.h"
#include "../parser.h"
#include "../linkedlist.h"
#include "../interpreter.h"


void connectedMode(SOCKET sock){
	header_t header = {-1,-1,-1}; // Servira de header de réception et d'envoi, il faudra juste changer la taille
	char* message = NULL;
	char* startMsg; // Permet de connaître la première adresse en mémoire du msg pour pouvoir la libérer par la suite
	char* responseForServ = NULL;
	
	linkedlist_t listResp = NULL; // La liste des réponses de commande 
	command_t command = {NULL, NULL};
	
	
	while(1){
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
			
			////////////////////////////////////////////////////////////////////////
			////							REPONSE								////
			////////////////////////////////////////////////////////////////////////
			if(responseForServ != NULL){
				header.size = strlen(responseForServ)+1;
				if(sendHeader(sock, header) == -1){
					perror("Erreur dans l'envoi de l'entete au serveur");
					exit(EXIT_FAILURE);
				}
				sendMessage(sock, responseForServ);
				free(responseForServ);
				responseForServ = NULL;
			}
		}
	}
		
		
		
}
