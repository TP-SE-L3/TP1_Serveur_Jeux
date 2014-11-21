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

/* /////////////////////////////////////////////////////////////////////////// 
	TODO :
		* Libérer la mémoire des noms de commande
		* Libérer la mémoire des chaine représentants les arguments des commandes (Dans parser.c)
	
	
/////////////////////////////////////////////////////////////////////////// */




/* Main coté Client */
int main(){
	SOCKET sock;
	header_t header = {-1,-1,-1}; // Servira de header de réception et d'envoi, il faudra juste changer la taille
	char* message = NULL;
	//~ char* startMsg; // Permet de connaître la première adresse en mémoire du msg pour pouvoir la libérer par la suite
	char* responseForServ = "Bonjour";
	
	//linkedlist_t listResp = NULL; // La liste des réponses de commande 
	//command_t command = {NULL, NULL};
	
	
	sock = connectCli("127.0.0.1");
	
	//while(1){
		//~ if(sendHeader(sock, header) == -1){
			//~ perror("Erreur dans l'envoi de l'entete au serveur");
			//~ exit(EXIT_FAILURE);
		//~ }
		header.size = strlen(responseForServ)+1;
		if(sendHeader(sock, header) == -1){
			perror("Erreur dans l'envoi de l'entete au serveur");
			exit(EXIT_FAILURE);
		}
		sendMessage(sock, "%s", responseForServ);
		//free(responseForServ);
		
		if(recvHeader(sock, &header) == -1){
			perror("Erreur dans la réception de l'entete");
			exit(EXIT_FAILURE);
		}
		if(header.size != -1){
			message = NULL;
			message = recvMessage(sock, header);
			printf("RECEPT message : %s\n", message);
			free(message);
		}
		
		
		if(shutdown(sock, SHUT_RDWR) == -1){
			perror("Erreur pendant la fermeture du socket.");
			exit(EXIT_FAILURE);
		}
		close(sock);
	
	
	return EXIT_SUCCESS;
}


