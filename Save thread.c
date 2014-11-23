#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <pthread.h>
//~ 
//~ #include "coreServ.h"
#include "../core.h"
//~ #include "../linkedlist.h"
//~ 
//~ /* Main coté Serveur */
//~ 
//~ /* /////////////////////////////////////////////////////////////////////////// 
	//~ TODO :
		//~ * Faire la conversion de float dans recupe respCli
		//~ * Libérer la mémoire des args dans getRespCli
		//~ * Changer la fonction sendMessage
		//~ * Remplacer le tableau listGame par une liste chainée
	//~ 
//~ /////////////////////////////////////////////////////////////////////////// */
//~ 
//~ 
//~ 
void* receptClient(void* arg);

int main(){
	
		SOCKET sockServ, sockCli;
		struct sockaddr_in addrServ;
		socklen_t lenAddrServ = sizeof(addrServ);
		int sockOptions = 1;
		pthread_t th_Cli;
		//~ 
		//~ 
		//~ linkedlist_t respCli = NULL;
		//~ char* msgToSend;
		//~ game_t listGames[SIZE_LIST_GAME];
		//~ int indiceListGame = 0;
		//~ 
		//~ 
		sockServ = socket(AF_INET, SOCK_STREAM, 0);
		if(sockServ == -1){ 
				perror("Error socket");
				exit(EXIT_FAILURE);
		}
		memset(&addrServ, 0, sizeof(addrServ));
		addrServ.sin_addr.s_addr = inet_addr("127.0.0.1");  // aussi : htonl(INADDR_ANY) -> toutes les addresses
		addrServ.sin_family = AF_INET;
		addrServ.sin_port = htons(PORT);
		//~ 
		// Permet de pouvoir réutiliser l'addresse du bind en relançant le programme
		// Empèche l'erreur -> Bind : Address already in use
		if(setsockopt(sockServ, SOL_SOCKET, SO_REUSEADDR, &sockOptions, sizeof(sockOptions)) == -1){
			perror("Error setsockopt");
			exit(EXIT_FAILURE);
		}
		//~ 
		if(bind(sockServ, (struct sockaddr*)&addrServ, sizeof(addrServ)) == -1){
				perror("Error Bind");
				exit(EXIT_FAILURE);
		}
		//~ 
		//~ /*FD_SET(sockServ, &readfs);
		//~ res = select(sockServ+1, &readfs, NULL, NULL, &timeout);*/
					//~ 
		if(listen(sockServ, 1) == -1){
			perror("Error Listen");
			exit(EXIT_FAILURE);
		}
		//~ 
		while(1){
			sockCli = accept(sockServ, (struct sockaddr*)&addrServ, &lenAddrServ);
			//~ if(sockCli == - 1){
				//~ perror("Error Accept");
			//~ }
			//~ printf("-> Client sur le socket : %d\n", (int)sockCli);
			//~ 
			
			if (pthread_create(&th_Cli, NULL, receptClient, (void*)sockCli)) { // Creation du thread
				perror("Err pthread_create");
				exit(-1);
			}
			
			
			
			//sleep(10);

		}

			//~ }
			//~ 
			//~ msgToSend = gameManager(&header.idGame, header.id, respCli, listGames, &indiceListGame);
			//~ respCli = cleanL(respCli, 0);
			//~ header.size = ((msgToSend == NULL)? (-1) : (signed int)strlen(msgToSend));
			//~ sendHeader(sockCli, header);
			//~ 
			//~ if(msgToSend != NULL){
				//~ header.size = strlen(msgToSend)+1;
				//~ sendMessage(sockCli, "%s", msgToSend);
				//~ free(msgToSend);
				//~ msgToSend = NULL;
			//}
			
			
		//~ }
		//~ 
		//~ 
		//~ 
		return 0;
}


void* receptClient(void* arg){
	printf("Thread !!!!\n");
	char* message;
	header_t header;
	SOCKET sockCli = (int)arg;
	char* msgToSend = NULL;
	//int currentId = 1; // L'id que l'on attribut aux client (A chaque fois qu'il y a un nouveau client, il est incrémenté)
	
	if(recvHeader(sockCli, &header) == -1){
		perror("Erreur dans la réception de l'entete d'un client");
		exit(EXIT_FAILURE);
	}
	//~ if(header.id == -1){ // Le client n'a pas encore d'ID, il faut lui en attribuer un
		//~ header.id = currentId;
		//~ currentId++;
	//~ }
	if(header.size != -1){ // Si -1, aucun msg à recevoir
		message = malloc(header.size);
		int res = recv(sockCli, message, header.size, 0);
		if(res != header.size-1){
			perror("Erreur de reception");
			exit(EXIT_FAILURE);
		}
		free(message);
		
		if(res == -1){
			perror("Error recv");
			exit(EXIT_FAILURE);
		}
		printf("RECEPT : message : %s\n", message);
		
		msgToSend = "Salut !";
		header.size = ((msgToSend == NULL)? (-1) : (signed int)strlen(msgToSend));
		if(header.size != -1){
			printf("MSG to send : %s et Size : %d\n", msgToSend, strlen(msgToSend));
			sendHeader(sockCli, header);
			sendMessage(sockCli, "%s", msgToSend);
			//free(msgToSend);
		}
		
		
		
		//~ respCli = getRespCli(message); // Récupère les réponses client
		//~ //printf("Msg : %s et header.id : %d\n", message, header.id);
		
		
		if(shutdown(sockCli, SHUT_RDWR) == -1){
			perror("Error to shutdown sockets");
			exit(EXIT_FAILURE);
		}
		close(sockCli);
	}
	
	return arg;
}
