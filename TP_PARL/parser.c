#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "parser.h"
#include "linkedlist.h"

command_t getCommand(char** str){
	char* resStr;
	char* arg = NULL;
	TypeArg_e resTypeArg;
	command_t command = {NULL, NULL};
	
	resStr = strstr(*str, TAG_COMMAND);
	if(resStr == NULL){ // Aucune commande n'a été trouvée
		return (command_t){NULL, NULL};
	}
	resStr += strlen(TAG_COMMAND);
	// Le nom de la commande est ici, il faut juste couper la chaine au bon endroit
	*str = substrpbrk(resStr, " ;\n"); // Ajoute le \0 à la fin de la commande
	command.name = malloc(strlen(resStr)+1);
	strcpy(command.name, resStr); // Enregistre la commande
	if(**str == '['){
		*str += 1;
		while(**str != ']' && **str != ';' && **str != '\0'){
			resTypeArg = recupArg(str, &arg);
			if(resTypeArg == A_ERROR){ // Argument introuvable
				break;
			}
			else if(resTypeArg == A_STRING){ // Ajoute une Chaine
				//printf("Chaine : %s et str: %s\n", arg, *str);
				command.args = addHeadL(command.args, (void*)arg, STRING);
			}
			else if(resTypeArg == A_INT){ // Ajoute une Chaine
				//printf("Chaine : %s et str: %s\n", arg, *str);
				command.args = addHeadL(command.args, (void*)atoi(arg), INT);
			}
			else{ // Ajoute un nombre
				printf("Nombre : %s et str: %s\n", arg, *str);
				//command.args = addHeadL(command.args, (void*)atoi(arg), INT);
			}
		}
	}
	
	return command;
}

// On utilise des doubles pointeurs car les adresses des chaines sont modifiées
TypeArg_e recupArg(char** str, char** arg){
	int i;
	int ret = A_ERROR; // Valeur de retour
	char* startArg = NULL;
	
	for(i=0; *(*(str)+i) == ' '; i++); // Supprime les espace devant s'il y en a
	*str += i;
	startArg = *str; // Le début de l'agrgument
	if(**str == '"'){ // Le paramètre est une chaine
		startArg += 1; // Enlève les premiers guillemets
		*str = strchr(*str+1, '"');
		ret = A_STRING;
	}
	else if(**str == ']' || **str == '\0'){ //Il n'y a pas de paramètres
		return A_ERROR;
	}
	else{ // Le paramètre est un nombre
		*str = strpbrk(*str+1, " ]");
		ret = A_INT;
	}
	
	if(**str != '\0'){ // Si on est pas à la fin de la chaine, coupe le caractère
		**str = '\0';
		*str = *str+1;
	}
	
	if(ret == A_INT && strchr(startArg, '.') != NULL){ // Si on trouve un point, c'est un Float
		ret = A_FLOAT;
	}
	
	if(startArg != NULL){
		*arg = malloc(strlen(startArg) + 1);
		strcpy(*arg, startArg);
	}
	
	return ret;
}


char* formatResponse(linkedlist_t* listResp){
	element* el;
	char bufNumber[15]; // Chaine de caractère qui permettra de savoir combien de caractères prend un nombre
	int sizeStr = 0;
	char* strResp = NULL; // La chaine de réponse
	
	if(*listResp == NULL){
		return NULL;
	}
	// Calcule la taille de la chaine de réponse
	for(el = *listResp; el != NULL; el = el->next){
		switch(el->type){ // switch au cas où l'on rajoute d'autre types plus tard
			case INT:
				sizeStr += sprintf(bufNumber, "%d", (int)el->val);
			break;
			case FLOAT:
				sizeStr += sprintf(bufNumber, "%f", *(float*)el->val);
			break;
			case STRING:
				sizeStr += strlen((char*)el->val) + 2; // +2: Les guillemets
			break;
			default: break;
		}
	}
	
	if(sizeStr != 0){
		sizeStr += sizeL(*listResp)-1 + 4; // sizeL-1 : Les espaces et 4: '[', ']', ';' '\0'
		
		strResp = malloc(sizeStr * sizeof(char));
		*strResp = '[';
		*(strResp+1) = '\0'; // Pour ne pas avoir de problème avec strcat
		while(!isEmptyL(*listResp)){
			el = popL(listResp);
			switch(el->type){
				case INT:
					sprintf(bufNumber, "%d", (int)el->val);
					strcat(strResp, bufNumber);
				break;
				case FLOAT:
					sprintf(bufNumber, "%f", *(float*)el->val);
					strcat(strResp, bufNumber);
					free((float*)el->val);
				break;
				case STRING:
					strcat(strResp, "\"");
					strcat(strResp, (char*)el->val);
					strcat(strResp, "\"");
					free(el->val);
				break;
				default: break;
			}
			free(el);
		}

		strcat(strResp, "];");
	}
	
	//printf("StrResp : %s et len : %d\n", strResp, strlen(strResp));
	
	return strResp;
}
