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


char* gameManager(int* idGame, int idCli, linkedlist_t args){
	//linkedlist_t listCommand = NULL;
	
	char* commands = NULL;
	
	
	if(*idGame == -1){
		int choix = 0;
		if(!isEmptyL(args)){
			element* el = popL(&args);
			choix = (int)el->val;
			switch(choix){
				case 1:
					outc(&commands, "%s", "Jeux 1\n");
				break;
				case 2:
					outc(&commands, "%s", "Quitter\n");
				break;
				default: outc(&commands, "%s", "Choix incorrect...\n\n"); break;
			}
		}
		if(choix == 0){
			outc(&commands, "%s", "=================== MENU ===================\n");
			outc(&commands, "%d %s", 1, ". Jeu 1\n");
			outc(&commands, "%d %s", 2, ". Quitter\n");
			inc(&commands, "%d");
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
			printf("Nombre -> %d\n", atoi(arg));
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
