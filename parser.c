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
	command.name = resStr; 
	*str = substrpbrk(resStr, " ;\n");
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
			else{ // Ajoute un nombre
				//printf("Nombre : %d et str: %s\n", atoi(arg), *str);
				command.args = addHeadL(command.args, (void*)atoi(arg), INT);
			}
		}
	}
	return command;
}

// On utilise des doubles pointeurs car les adresses des chaines sont modifiées
TypeArg_e recupArg(char** str, char** startArg){
	int i;
	int ret = A_ERROR; // Valeur de retour
	for(i=0; *(*(str)+i) == ' '; i++); // Supprime les espace devant s'il y en a
	*str += i;
	*startArg = *str; // Le début de l'agrgument
	if(**str == '"'){ // Le paramètre est une chaine
		*startArg += 1; // Enlève les premiers guillemets
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
	return ret;
}


char* formatResponse(linkedlist_t* listResp){
	element* el;
	char bufNumber[10]; // Chaine de caractère qui permettra de savoir combien de caractères prend un nombre
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
		while(!isEmptyL(*listResp)){
			el = popL(listResp);
			switch(el->type){
				case INT:
					sprintf(bufNumber, "%d", (int)el->val);
					strcat(strResp, bufNumber);
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
	
	printf("StrResp : %s et len : %d\n", strResp, strlen(strResp));
	
	return strResp;
}
