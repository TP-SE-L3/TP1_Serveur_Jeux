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
		* Faire la conversion de float dans recupe respCli
		* Libérer la mémoire des args dans getRespCli
		* Changer la fonction sendMessage
		* Remplacer le tableau listGame par une liste chainée
	
/////////////////////////////////////////////////////////////////////////// */



int main(){
		SOCKET sockServ, sockCli;
		struct sockaddr_in addrServ;
		socklen_t lenAddrServ = sizeof(addrServ);
		int sockOptions = 1;
		char message[256];
		header_t header;
		
		
		linkedlist_t respCli = NULL;
		char* msgToSend;
		int currentId = 1; // L'id que l'on attribut aux client (A chaque fois qu'il y a un nouveau client, il est incrémenté)
		game_t listGames[SIZE_LIST_GAME];
		int indiceListGame = 0;
		
		game_t test = {1,1,1,1,1};
		
		/*fd_set readfs;
		FD_ZERO(&readfs);
		struct timeval timeout;
		timeout.tv_sec = 20;
		timeout.tv_usec = 0;*/
		
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
		
		/*FD_SET(sockServ, &readfs);
		res = select(sockServ+1, &readfs, NULL, NULL, &timeout);*/
					
		if(listen(sockServ, 5) == -1){
			perror("Error Listen");
			exit(EXIT_FAILURE);
		}
		//listGames[0] = test;
		
		while(1){
			sockCli = accept(sockServ, (struct sockaddr*)&addrServ, &lenAddrServ);
			if(sockCli == - 1){
				perror("Error Accept");
			}
			printf("-> Client sur le socket : %d\n", (int)sockCli);
			
			
			if(recvHeader(sockCli, &header) == -1){
				perror("Erreur dans la réception de l'entete d'un client");
				exit(EXIT_FAILURE);
			}
			if(header.id == -1){ // Le client n'a pas encore d'ID, il faut lui en attribuer un
				header.id = currentId;
				currentId++;
			}
			if(header.size != -1){ // Si -1, aucun msg à recevoir
				if(recv(sockCli, message, header.size, 0) == -1){
					perror("Error recv");
					exit(EXIT_FAILURE);
				}
				respCli = getRespCli(message); // Récupère les réponses client
				printf("Msg : %s et header.id : %d\n", message, header.id);
			}
			
			msgToSend = gameManager(&header.idGame, header.id, respCli, listGames, &indiceListGame);
			respCli = cleanL(respCli, 1);
			if(msgToSend != NULL){
				header.size = strlen(msgToSend)+1;
				sendHeader(sockCli, header);
				sendMessage(sockCli, "%s", msgToSend);
				free(msgToSend);
			}
			
			
			if(shutdown(sockCli, SHUT_RDWR) == -1){
				perror("Error to shutdown sockets");
				exit(EXIT_FAILURE);
			}
			close(sockCli);
		}
		
		
		
		return 0;
}
