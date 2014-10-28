#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	return listResp;
}


void performOut(linkedlist_t *args){
	element* el = NULL;
	
	while(!isEmptyL(*args)){
		el = popL(args);
		if(el->type == STRING){
			printf("%s", (char*)el->val);
		}
		else if(el->type == INT){
			printf("%d", (int)el->val);
		}
		free(el);
	}
}

linkedlist_t performIn(linkedlist_t *args, linkedlist_t listResp){
	element* el = NULL;
	char* val;
	int size = 10; // Si le scanf est une chaine, on défini la taille chaine entrée
	int resInt;
	char* resChar = NULL;
	float resFloat;
	
	if(!isEmptyL(*args)){
		el = popL(args);
		if(el->type == STRING){
			val = (char*)el->val;
			if(strcmp(val, "%s") == 0){ // STRING
				el = popL(args);
				if(el != NULL && el->type == INT){ // Dans le cas d'une chaine, il faut allouer de la mémoire
					size = (int)el->val;
				}
				resChar = malloc(size * sizeof(char)); // Il faudra libérer la mémoire après
				scanf("%s", resChar);
				listResp = addHeadL(listResp, (void*)resChar, STRING);
			}
			else if(strcmp(val, "%d") == 0){;
				scanf("%d", &resInt);
				listResp = addHeadL(listResp, (void*)resInt, INT);
			}
			else if(strcmp(val, "%f") == 0){ // Il est difficile de récupérer un float, ne pas les privilégier
				scanf("%f", &resFloat);
				listResp = addHeadL(listResp, &resFloat, FLOAT); // Attention ici c'est l'adresse qui est enregistrée !!!!
			}
		}
	}
	return listResp;
}
