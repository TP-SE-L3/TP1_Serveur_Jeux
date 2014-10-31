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


char* gameManager(int* idGame, int idCli, linkedlist_t args){
	linkedlist_t listCommand = NULL;
	
	//char* commands = "cmd:out [\"Entrez un nombre : \"]; cmd:in [ \"%d\"];";
	char* commands = NULL;
	//~ listCommand = outc(listCommand,"%s %d %s", "Entrez le nombre ", 2, ".3 : ");
	listCommand = inc(listCommand, "%s");
	//~ 
	//~ if(*idGame == -1){
		//~ 
	//~ }
	//~ 
	//~ printf("Command size : %d\n", sizeL(listCommand));
	commands = listToStringL(listCommand);
	listCommand = cleanL(listCommand); // Vide la liste
	//~ printf("Commande : %s\n", commands);
	
	return commands;
}

linkedlist_t outc(linkedlist_t listCommand, char* format, ...){
	int i, j;
	// nwFormat : Rajoute des éléments pour que la commande soit valide
	char nwFormat[strlen(format) + (nbstr(format, "%s")*2) + 13]; // *2 la place pour le "" entre les chaines, +10 pour Pour "cmd:out [" et "]; " et '\0'
	char* command;
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
	printf("Format : %s", nwFormat);
	
	int taille = vsnprintf(NULL, 0, nwFormat, listArgs);
	va_end(listArgs);
	
	command = malloc(taille+1); // +1 pour \0
	
	va_start(listArgs, format);
	vsnprintf(command, taille+1, nwFormat, listArgs);
	va_end(listArgs);
	return addQueueL(listCommand, (void*)command, STRING); // ajoute la comande à la liste
}

linkedlist_t inc(linkedlist_t listCommand, char* format){ 
	char* command = malloc(16); // 16 : Pour écrire cmd:in ["%n"];\n\0"
	char* tmpStr;
	strcat(command, "cmd:in [\"%");
	tmpStr = strchr(format, '%');
	command[strlen(command)] = *(tmpStr+1);
	strcat(command, "\"];\n");
	return addQueueL(listCommand, (void*)command, STRING); // ajoute la comande à la liste
}
