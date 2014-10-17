#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "parser.h"


command_t getCommand(char* str){
	char* resStr;
	char* arg = NULL;
	command_t command;
	resStr = strstr(str, TAG_COMMAND);
	if(resStr == NULL){
		return (command_t){NULL, NULL};
	}
	resStr += strlen(TAG_COMMAND);
	// Le nom de la commande est ici, il faut juste couper la chaine au bon endroit
	command.name = resStr;
	//*resStr = '\0'; 
	printf("resStr: %s\n", resStr);
	str = substrpbrk(resStr, " ;\n");
	if(*(str) == '['){
		str++;
		if(recupArg(&str, &arg) == -1){
			printf("Aucun argument\n");
		}
		else{
			if(*arg == '"'){
				//*str='\0';
				printf("Chaine : %s et str: %s\n", arg, str);
			}
			else{
				//*str='\0';
				printf("Nombre : %s et str: %s\n", arg, str);
			}
		}
		if(*str == ']' || *str == ';'){
			printf("Fin de la commande\n");
		}
		command.strArgs = resStr;
	}
	else{
		command.strArgs = NULL;
	}
	return command;
}

int recupArg(char** str, char** startArg){
	int i;
	for(i=0; *(*(str)+i) == ' '; i++); // Supprime les espace devant s'il y en a
	*str += i;
	*startArg = *str; // Le début de l'agrgument
	if(**str == '"'){ // Le paramètre est une chaine
		*str = strchr(*str+1, '"');
	}
	else if(**str == ']' || **str == '\0'){ //Il n'y a pas de paramètres
		return -1;
	}
	else{ // Le paramètre est un nombre
		*str = strpbrk(*str+1, " ]");
	}
	
	if(**str != '\0'){ // Si on est pas à la fin de la chaine, coupe le caractère
		**str = '\0';
		*str = *str+1;
	}
	return 0;
}
