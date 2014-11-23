#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#include "coreServ.h"
#include "../core.h"
#include "../linkedlist.h"

/* Main coté Serveur */

/* /////////////////////////////////////////////////////////////////////////// 
	TODO :
		* Commande pour éffacer la console
		* Utiliser les fonctions : ConnectServ et ConnnectCli
		* Penser à vider la mémoire, de la liste chainée, une fois qu'une partie se termine
	
/////////////////////////////////////////////////////////////////////////// */



int main(){
		SOCKET sockServ, sockCli;
		struct sockaddr_in addrServ;
		socklen_t lenAddrServ = sizeof(addrServ);
		int sockOptions = 1;
		char* message;
		header_t header;
		
		linkedlist_t respCli = NULL;
		char* msgToSend;
		int currentId = 1; // L'id que l'on attribut aux client (A chaque fois qu'il y a un nouveau client, il est incrémenté)
		linkedlist_t listGames = NULL;
		int indiceListGame = 0;
		int closeSock = TRUE; // Boolean, si vrai on ferme le socket actuel, sinon on ne le ferme pas (il est dans un thread)
		
		
		sockServ = socket(AF_INET, SOCK_STREAM, 0);
		if(sockServ == -1){ 
				perror("Error socket");
				exit(EXIT_FAILURE);
		}
		memset(&addrServ, 0, sizeof(addrServ));
		addrServ.sin_addr.s_addr = inet_addr("127.0.0.1");  // aussi : htonl(INADDR_ANY) -> toutes les addresses
		addrServ.sin_family = AF_INET;
		addrServ.sin_port = htons(PORT);
		
		// Permet de pouvoir réutiliser l'addresse du bind en relançant le programme
		// Empèche l'erreur -> Bind : Address already in use
		if(setsockopt(sockServ, SOL_SOCKET, SO_REUSEADDR, &sockOptions, sizeof(sockOptions)) == -1){
			perror("Error setsockopt");
			exit(EXIT_FAILURE);
		}
		
		if(bind(sockServ, (struct sockaddr*)&addrServ, sizeof(addrServ)) == -1){
				perror("Error Bind");
				exit(EXIT_FAILURE);
		}
		
					
		if(listen(sockServ, 10) == -1){
			perror("Error Listen");
			exit(EXIT_FAILURE);
		}
		printf(" Le serveur est en marche.\n\n");
		while(1){
			sockCli = accept(sockServ, (struct sockaddr*)&addrServ, &lenAddrServ);
			closeSock = TRUE;
			if(sockCli == - 1){
				perror("Error Accept");
			}
			
			if(recvHeader(sockCli, &header) == -1){
				perror("Erreur dans la réception de l'entete d'un client");
				exit(EXIT_FAILURE);
			}
			// Nouveau client
			if(header.id == -1){ // Le client n'a pas encore d'ID, il faut lui en attribuer un
				header.id = currentId;
				currentId++;
				printf(" [ID:NvCli(%d) | Socket:%d | Partie:Aucune]\n", header.id, (int)sockCli);
			}
			else{
				printf(" [ID:%d | Socket:%d | Partie:", header.id, (int)sockCli);
				if(header.idGame == -1){
					printf("Aucune]\n");
				}
				else{
					printf("%d]\n", header.idGame);
				}
			}
			// Si le client envoie un message
			if(header.size != -1){ // Si -1, aucun msg à recevoir
				message = malloc(header.size);
				int res = recv(sockCli, message, header.size, 0);
				message[header.size-1] = '\0'; // Met le caractère '\0' en fin de chaine
				if(res != header.size-1){
					perror("Erreur de reception");
					exit(EXIT_FAILURE);
				}
				
				if(res == -1){
					perror("Error recv");
					exit(EXIT_FAILURE);
				}
				respCli = getRespCli(message); // Récupère les réponses client
				
				free(message);
			}
			player_t playerCli = {header.id, sockCli}; // Informations sur le client
			int idGameBefore = header.idGame;
			msgToSend = gameManager(&header.idGame, playerCli, respCli, &listGames, &indiceListGame);
			// Nouvelle partie, on est entré dans un thread ou on attend un autre joueur
			if(idGameBefore != header.idGame && header.idGame >= 0){
				closeSock = FALSE; // On ne ferme pas la socket
			}
			respCli = cleanL(respCli, 0);
			header.size = ((msgToSend == NULL)? (-1) : (signed int)strlen(msgToSend));
			sendHeader(sockCli, header);
			
			if(msgToSend != NULL){
				header.size = strlen(msgToSend)+1;
				sendMessage(sockCli, msgToSend);
				free(msgToSend);
				msgToSend = NULL;
			}
			
			if(closeSock){
				if(shutdown(sockCli, SHUT_RDWR) == -1){
					perror("Error to shutdown sockets");
					exit(EXIT_FAILURE);
				}
				close(sockCli);
			}
		}
		
		
		
		return 0;
}
