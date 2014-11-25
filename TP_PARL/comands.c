#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

#include "linkedlist.h"
#include "utils.h"

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
		if(*(format+i) == '%' && (*(format+i+1) == 's' || *(format+i+1) == 'c')){
			nwFormat[j++] = '"';
			nwFormat[j++] = '%';
			nwFormat[j++] = *(format+i+1);
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
	char* command = malloc(14 + strlen(timeToStr)); // 14 : Pour écrire cmd:wait [%n];\n\0"
	
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

void systemc(char** listCommand, char* commandSys){
	char* nwListCommand = NULL;
	int sizeListCmd = (*listCommand == NULL)? 0 : strlen(*listCommand);
	char* command = malloc(18 + strlen(commandSys)); // 18 : Pour écrire cmd:system ["%n"];\n\0"
	
	sprintf(command, "cmd:system [\"%s\"];\n", commandSys);
	
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


void quitc(char** listCommand){
	char* nwListCommand = NULL;
	int sizeListCmd = (*listCommand == NULL)? 0 : strlen(*listCommand);
	char command[14] = "cmd:quit [];\n";
	
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
