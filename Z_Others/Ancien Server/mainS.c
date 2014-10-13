#include <stdlib.h>
#include <stdio.h>

#include "coreS.h"
/* Main coté Serveur */

int main(){
		char choice[50] = "";
		int continu = 1;
		int res = 0;
		SOCKET sock;
		char* fmt = "Nom : %s";
		
		while(continu == 1){
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
						sock = CreeSocketServeur("1025");	
						//printf("Socket : %d\n", res);
					break;
					case '2':
						sendMessage(sock, fmt, "nom");
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
		}
		return 0;
}
