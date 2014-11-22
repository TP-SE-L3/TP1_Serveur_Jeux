#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

#include "coreServ.h"
#include "../linkedlist.h"
#include "../utils.h"
#include "../parser.h"
#include "../commands.h"


char* gameManager(int* idGame, int idCli,linkedlist_t args, linkedlist_t* listGames, int* currentIndex){
	//linkedlist_t listCommand = NULL;
	
	char* commands = NULL;
	int choix = 0;
	printf("List game size %d\n", sizeL(*listGames));
	
	if(*idGame == -1){
		if(!isEmptyL(args)){
			element* el = popL(&args);
			choix = (int)el->val;
			switch(choix){
				case GAME_PENDU:
					outc(&commands, "%s", "Pendu\n");
				break;
				case GAME_MORPION:
					outc(&commands, "%s", "Morpion\n");
				break;
				case CHOICE_EXIT:
					outc(&commands, "%s", "Quitter\n");
				break;
				default: 
					outc(&commands, "%s", "Choix incorrect...\n\n"); 
					waitc(&commands, 1);
					choix = 0; 
				break;
			}
			if(choix != 0 && choix != CHOICE_EXIT){
				element* el;
				for(el = *listGames; el != NULL; el = el->next){
					
					game_t* curGame = (game_t*)el->val;
					printf("Type G : %d\n", curGame->idTypeGame);
					if(curGame->idTypeGame == choix && curGame->nbPlayer < 2){
						
						*idGame = curGame->id;
						curGame->nbPlayer++;
						outc(&commands, "%s", "La partie peut commencer...\n");
						waitc(&commands, 15); // On attend pour le moment		
						break;
					}
				}
				
				
				if(*idGame == -1){
					game_t *nwGameP = malloc(sizeof(game_t)); // Le pointeur à enregistrer dans la liste
					nwGameP->id = *currentIndex;
					nwGameP->idTypeGame = choix;
					nwGameP->nbPlayer = 1;
					nwGameP->pipeW = 0;
					nwGameP->pipeR = 0;
					printf("Type nwGame : %d\n", nwGameP->idTypeGame);
					*listGames = addHeadL(*listGames, nwGameP, OTHER);
					*currentIndex += 1;
					*idGame = nwGameP->id;
					outc(&commands, "%s", "En attente d'un autre joueur.\n");
					waitc(&commands, 5);
				}
			}
		}
		////////////////////////////////////////////////////////////////////////
		////							MENU								////
		////////////////////////////////////////////////////////////////////////
		if(choix == 0){
			systemc(&commands, "clear");
			outc(&commands, "%s", "=================== MENU ===================\n");
			outc(&commands, "%d %s", GAME_PENDU, ". Pendu\n");
			outc(&commands, "%d %s", GAME_MORPION, ". Morpion\n");
			outc(&commands, "%d %s", CHOICE_EXIT, ". Quitter\n");
			inc(&commands, "%d");
		}
	}
	else{ // Si idGame != -1 Càd si le joueur est déjà dans une partie
		////////////////////////////////////////////////////////////////////////
		////					Parties en cours (ou attente)				////
		////////////////////////////////////////////////////////////////////////
		element* el;
		game_t curGame;
		for(el = *listGames; el != NULL; el = el->next){
			curGame = *(game_t*)el->val;
			if(curGame.id == *idGame){
				break;
			}
		}
		if(el == NULL){
			outc(&commands, "%s", "Erreur : Impossible de trouver la partie.\n");
			*idGame = -1;
		}
		else{
			if(curGame.nbPlayer < 2){
				outc(&commands, "%s", "."); // On attend toujours un autre joueur
				waitc(&commands, 5);
			}
			else{
				outc(&commands, "%s", "La partie peut commencer...\n");
				waitc(&commands, 15); // On attends pour le moment
			}
		}
	}
	
	
	/*commands = listToStringL(listCommand);
	listCommand = cleanL(listCommand, 1); // Vide la liste*/
	
	return commands;
}



linkedlist_t getRespCli(char* resp){
	linkedlist_t list = NULL;
	char* str;
	char* arg = NULL;
	TypeArg_e resTypeArg;
	
	str = strchr(resp, '[');
	if(str == NULL){
		return NULL;
	}

	str += 1;
	while(*str != ']' && *str != ';' && *str != '\0'){
		resTypeArg = recupArg(&str, &arg);
		if(resTypeArg == A_ERROR){ // Argument introuvable
			break;
		}
		else if(resTypeArg == A_STRING){ // Ajoute une Chaine
			list = addHeadL(list, (void*)arg, STRING);
		}
		else if(resTypeArg == A_INT){ // Ajoute un nombre
			list = addHeadL(list, (void*)atoi(arg), INT);
			free(arg);
		}
		else{ // Ajoute un float
			float* respFloat = malloc(sizeof(float));
			*respFloat = (float)atof(arg);
			list = addHeadL(list, respFloat, FLOAT);
			free(arg);
		}
	}
	return list;
}
