#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>


#include "../core.h"

/* Main coté Serveur */

int main(){
		SOCKET sockServ, sockCli;
		struct sockaddr_in addrServ;
		socklen_t lenAddrServ = sizeof(addrServ);
		int sockOptions = 1;
		char message[256];
		header_t header = {1, 10};
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
		
		while(1){
			sockCli = accept(sockServ, (struct sockaddr*)&addrServ, &lenAddrServ);
			if(sockCli == - 1){
					perror("Error Accept");
			}
			printf("-> Nouveau client sur le socket : %d", (int)sockCli);
			
			
			if(recv(sockCli, message, sizeof(message), 0) == -1){
				perror("Error recv");
				exit(EXIT_FAILURE);
			}
			
			printf("Msg : %s\n", message);
			

			sendHeader(sockCli, header);
			//sendMessage(sockCli, format, "Mon messages est le suivant...\0");
			
			
			
			if(shutdown(sockCli, SHUT_RDWR) == -1){
				perror("Error to shutdown sockets");
				exit(EXIT_FAILURE);
			}
			close(sockCli);
		}
		
		/*char choice[50] = "";
		int continu = 1;*/
		/*while(continu == 1){
			//system("clear");
			printf("======== MENU PRINCIPAL =========\n");
			printf("A quel jeux voulez-vous jouer ?\n");
			printf(" 1. Connexion\n");
			printf(" 2. Msg\n");
			printf(" 3. Bataille Naval\n");
			printf(" 4. Quitter\n");
			scanf("%s", choice);
			switch(choice[0]){
					case '1':
					break;
					case '2':
					break;
					case '3':
						printf(" Bataill\n");
					break;
					case '4':
						continu = 0;
					break;
					default:
						printf("Votre choix est invlide.\n");
					break;
			}
		}*/
		
		
		return 0;
}
