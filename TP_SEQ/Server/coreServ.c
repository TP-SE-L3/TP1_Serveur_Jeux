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


char* gameManager(int* idGame, int idCli,linkedlist_t args, game_t listGames[SIZE_LIST_GAME], int* currentIndex){
	//linkedlist_t listCommand = NULL;
	printf("---------------------\n");
	
	char* commands = NULL;
	int choix = 0;
	
	
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
					choix = 0; 
				break;
			}
			if(choix != 0 && choix != CHOICE_EXIT){
				int i;
				
				for(i=0; i < *currentIndex; i++){
					// On à trouvé une partie de libre
					if(listGames[i].idTypeGame == choix && listGames[i].nbPlayer < 2){
						*idGame = listGames[i].id;
						listGames[i].nbPlayer++;
						outc(&commands, "%s", "La partie peut commencer...\n");
						waitc(&commands, 10); // On attend pour le moment		
						break;
					}
				}
				
				if(*idGame == -1){ // Si aucune partie disponible
					game_t nwGame = {*currentIndex, choix, 1, 0, 0};
					listGames[*currentIndex] = nwGame;
					*currentIndex += 1;
					*idGame = nwGame.id;
					outc(&commands, "%s", "En attente d'un autre joueur.\n");
					waitc(&commands, 10);
				}
			}
		}
		////////////////////////////////////////////////////////////////////////
		////							MENU								////
		////////////////////////////////////////////////////////////////////////
		if(choix == 0){
			waitc(&commands, 1);
			outc(&commands, "%s", "=================== MENU ===================\n");
			outc(&commands, "%d %s", GAME_PENDU, ". Pendu\n");
			outc(&commands, "%d %s", GAME_MORPION, ". Morpion\n");
			outc(&commands, "%d %s", CHOICE_EXIT, ". Quitter\n");
			inc(&commands, "%d");
		}
	}
	else{ // Si idGame != -1 Càd si le joueur est déjà dans une partie
		int i;
		for(i=0; i < *currentIndex && listGames[i].id != *idGame; i++);
		if(i == *currentIndex){
			outc(&commands, "%s", "Erreur : Impossible de trouver la partie.\n");
			*idGame = -1;
		}
		else{
			if(listGames[i].nbPlayer < 2){
				outc(&commands, "%s", "."); // On attend toujours un autre joueur
				waitc(&commands, 10);
			}
			else{
				outc(&commands, "%s", "La partie peut commencer...\n");
				waitc(&commands, 30); // On attends pour le moment
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
		}
		else{ // Ajoute un float
			printf("==> Float %s\n", arg);
			//list = addHeadL(list, (void*)atoi(arg), INT);
		}
	}
	return list;
}



void outc(char** listCommand, char* format, ...){
	int i, j;
	// nwFormat : Rajoute des éléments pour que la commande soit valide
	char* nwListCommand = NULL;
	char* command = NULL;
	char nwFormat[strlen(format) + (nbstr(format, "%s")*2) + 13]; // *2 la place pour le "" entre les chaines, +10 pour Pour "cmd:out [" et "]; " et '\0'
	int sizeListCmd = (*listCommand == NULL)? 0 : strlen(*listCommand);
	
	va_list listArgs;
	va_start(listArgs, format);
	strcpy(nwFormat, "cmd:out [");
	for(i=0, j=strlen(nwFormat); format[i] != '\0'; i++){ // Rajoute les "" pour les chaines
		if(*(format+i) == '%' && *(format+i+1) == 's'){
			nwFormat[j++] = '"';
			nwFormat[j++] = '%';
			nwFormat[j++] = 's';
			nwFormat[j++] = '"';
			i++;
		}
		else{
			nwFormat[j] = format[i];
			j++;
		}
	}
	nwFormat[j] = '\0';
	strcat(nwFormat, "];\n");
	
	int taille = vsnprintf(NULL, 0, nwFormat, listArgs);
	va_end(listArgs);
	
	command = malloc(taille+1); // +1 pour \0
	
	va_start(listArgs, format);
	vsnprintf(command, taille+1, nwFormat, listArgs);
	va_end(listArgs);
	nwListCommand = malloc(taille + sizeListCmd + 1);
	if(*listCommand != NULL){
		strcpy(nwListCommand, *listCommand);
		free(*listCommand);
	}
	else{
		*nwListCommand = '\0';
	}
	strcat(nwListCommand, command);
	*listCommand = nwListCommand;
}


void inc(char** listCommand, char* format){ 
	char* command = malloc(16); // 16 : Pour écrire cmd:in ["%n"];\n\0"
	char* tmpStr;
	char* nwListCommand = NULL;
	int sizeListCmd = (*listCommand == NULL)? 0 : strlen(*listCommand);
	strcpy(command, "cmd:in [\"%?"); // Le ? va être remplacé
	tmpStr = strchr(format, '%');
	command[strlen(command)-1] = *(tmpStr+1);
	strcat(command, "\"];\n");
	command[15] = '\0';
	
	nwListCommand = malloc(15 + sizeListCmd + 1);
	
	if(*listCommand != NULL){
		strcpy(nwListCommand, *listCommand);
		free(*listCommand);
	}
	else{
		*nwListCommand = '\0';
	}
	strcat(nwListCommand, command);
	*listCommand = nwListCommand;
}



void waitc(char** listCommand, int time){
	char* nwListCommand = NULL;
	int sizeListCmd = (*listCommand == NULL)? 0 : strlen(*listCommand);
	char timeToStr[10];
	sprintf(timeToStr, "%d", time);
	char* command = malloc(14 + strlen(timeToStr)); // 16 : Pour écrire cmd:wait ["%n"];\n\0"
	
	sprintf(command, "cmd:wait [%s];\n", timeToStr);
	
	nwListCommand = malloc(strlen(command) + sizeListCmd + 1);
	
	if(*listCommand != NULL){
		strcpy(nwListCommand, *listCommand);
		free(*listCommand);
	}
	else{
		*nwListCommand = '\0';
	}
	strcat(nwListCommand, command);
	*listCommand = nwListCommand;
}


//~ void outc2(linkedlist_t *listCommand, char* format, ...){
	//~ int i, j;
	//~ // nwFormat : Rajoute des éléments pour que la commande soit valide
	//~ char nwFormat[strlen(format) + (nbstr(format, "%s")*2) + 13]; // *2 la place pour le "" entre les chaines, +10 pour Pour "cmd:out [" et "]; " et '\0'
	//~ char* command = NULL;
	//~ va_list listArgs;
	//~ va_start(listArgs, format);
	//~ strcpy(nwFormat, "cmd:out [");
	//~ for(i=0, j=strlen(nwFormat); format[i] != '\0'; i++){ // Rajoute les "" pour les chaines
		//~ if(*(format+i) == '%' && *(format+i+1) == 's'){
			//~ nwFormat[j++] = '"';
			//~ nwFormat[j++] = '%';
			//~ nwFormat[j++] = 's';
			//~ nwFormat[j++] = '"';
			//~ i++;
		//~ }
		//~ else{
			//~ nwFormat[j] = format[i];
			//~ j++;
		//~ }
	//~ }
	//~ nwFormat[j] = '\0';
	//~ strcat(nwFormat, "];\n");
	//~ printf("Format : %s", nwFormat);
	//~ 
	//~ int taille = vsnprintf(NULL, 0, nwFormat, listArgs);
	//~ va_end(listArgs);
	//~ 
	//~ command = malloc(taille+1); // +1 pour \0
	//~ 
	//~ va_start(listArgs, format);
	//~ vsnprintf(command, taille+1, nwFormat, listArgs);
	//~ va_end(listArgs);
	//~ *listCommand = addQueueL(*listCommand, (void*)command, STRING); // ajoute la comande à la liste
//~ }
//~ 
//~ void inc2(linkedlist_t *listCommand, char* format){ 
	//~ char* command = malloc(16); // 16 : Pour écrire cmd:in ["%n"];\n\0"
	//~ char* tmpStr;
	//~ strcpy(command, "cmd:in [\"%?"); // Le ? va être remplacé
	//~ tmpStr = strchr(format, '%');
	//~ command[strlen(command)-1] = *(tmpStr+1);
	//~ strcat(command, "\"];\n");
	//~ command[15] = '\0';
	//~ 
	//~ printf("IN COMMAND MI -> %s  len: %d\n", command, strlen(command));
	//~ *listCommand = addQueueL(*listCommand, (void*)command, STRING); // ajoute la comande à la liste
//~ }
