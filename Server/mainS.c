#include <stdlib.h>
#include <stdio.h>

#include "coreS.h"
/* Main coté client */

int main(){
		char choice[0];
		int continu = 1;
		
		while(continu == 1){
			system("clear");
			printf("======== MENU PRINCIPAL =========");
			printf("A quel jeux voulez-vous jouer ?\n");
			printf(" 1. Pendu\n");
			printf(" 2. Morpion\n");
			printf(" 3. Bataille Naval\n");
			printf(" 4. Quitter");
			scanf("%s", choice);
			switch(choice[0]){
					case '1':
						printf("pendu");
					break;
					case '2':
						printf(" 2. Morpion\n");
					break;
					case '3':
						printf(" 2. Bataill\n");
					break;
					case '4':
						continu = 0;
					break;
			}
		}
}
