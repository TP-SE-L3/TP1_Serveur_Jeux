#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "interpreter.h"
#include "parser.h"
#include "linkedlist.h"

linkedlist_t performCommandCli(command_t *command, linkedlist_t listResp){
	if(command->name == NULL){
		return listResp;
	}
	if(strcmp(command->name, "out") == 0){
		performOut(&command->args);
	}
	else if(strcmp(command->name, "in") == 0){
		listResp = performIn(&command->args, listResp);
	}
	else if(strcmp(command->name, "wait") == 0){ // Attend pendant un certain temps
		performWait(&command->args);
	}
	else if(strcmp(command->name, "system") == 0){
		performSyst(&command->args);
	}
	return listResp;
}


void performOut(linkedlist_t *args){
	element* el = NULL;
	
	while(!isEmptyL(*args)){
		el = popL(args);
		if(el->type == STRING){
			printf("%s", (char*)el->val);
			free(el->val); // Libère la mémoire quand on à une chaine
		}
		else if(el->type == INT){
			printf("%d", (int)el->val);
		}
		free(el);
	}
	fflush(stdout);
}

linkedlist_t performIn(linkedlist_t *args, linkedlist_t listResp){
	element* el = NULL;
	char* val;
	int size = 25; // Si le scanf est une chaine, on défini la taille chaine entrée
	int resInt;
	char* resChar = NULL;
	float* resFloat = NULL;
	
	if(!isEmptyL(*args)){
		el = popL(args);
		if(el->type == STRING){
			val = (char*)el->val;
			free(el);
			if(strcmp(val, "%s") == 0){ // STRING
				el = popL(args);
				if(el != NULL && el->type == INT){ // Dans le cas d'une chaine, il faut allouer de la mémoire
					size = (int)el->val;
				}
				resChar = malloc(size * sizeof(char)); // Il faudra libérer la mémoire après
				char regexSc[7]; // Permet de sécuriser le scanf sur la taille
				sprintf(regexSc, "-%ds", size);
				regexSc[0] = '%';

				scanf(regexSc, resChar);
				listResp = addHeadL(listResp, (void*)resChar, STRING);
				free(el);
			}
			else if(strcmp(val, "%d") == 0){ // INT
				scanf("%d", &resInt);
				listResp = addHeadL(listResp, (void*)resInt, INT);
			}
			else if(strcmp(val, "%f") == 0){
				resFloat = malloc(sizeof(float));
				scanf("%f", resFloat);
				listResp = addHeadL(listResp, resFloat, FLOAT);
			}
			free(val);
		}
	}
	return listResp;
}

void performWait(linkedlist_t *args){
	element* el = NULL;
	if(!isEmptyL(*args)){
		el = popL(args);
		if(el->type == INT){
			sleep((int)el->val); // On attend le nombre de secondes données par le client
		}
		free(el);
	}
}

void performSyst(linkedlist_t *args){
	element* el = NULL;
	char* val;
	if(!isEmptyL(*args)){
		el = popL(args);
		if(el->type == STRING){
			val = (char*)el->val;
			if(strcmp("clear", val) == 0){
				system("clear");
			}
			free(val); // Libère la chaine
		}
		free(el);
	}
}
