#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"


linkedlist_t addHeadL(linkedlist_t list, void* val, Type_e typeVal){
	element* nwElem = malloc(sizeof(nwElem));
	nwElem->val = val;
	nwElem->type = typeVal;
	nwElem->next = list;
	return nwElem;
}

linkedlist_t addQueueL(linkedlist_t list, void* val, Type_e typeVal){
	element* nwElem = malloc(sizeof(nwElem));
	element* tmp;
	nwElem->val = val;
	nwElem->type = typeVal;
	nwElem->next = NULL;
	if(isEmptyL(list)){ // La liste est vide
		return nwElem;
	}
	for(tmp = list; tmp->next != NULL; tmp = tmp->next);
	tmp->next = nwElem;
	return list;
}

linkedlist_t addElemHeadL(linkedlist_t list, element* elem){
	elem->next = list;
	return elem;
}

element* popL(linkedlist_t* list){
	element* tmp;
	element* elemPre = NULL; // L'élément courrant précédent
	
	if(isEmptyL(*list)){
		return NULL;
	}
	
	for(tmp = *list; tmp->next != NULL; tmp = tmp->next){
		elemPre = tmp;
	}
	if(elemPre == NULL){
		*list = NULL; // La liste est vide
	}
	else{
		elemPre->next = NULL;
	}
	return tmp;
}

int isEmptyL(linkedlist_t list){
	return (list == NULL)? 1 : 0;
}

int sizeL(linkedlist_t list){
	int i;
	element* tmp;
	if(isEmptyL(list)){
		return 0;
	}
	for(tmp = list, i=1; tmp->next != NULL; tmp = tmp->next, i++);
	return i;
}

linkedlist_t cleanL(linkedlist_t list, int freeStr){
	element* tmp;
	element* elemNext;
	
	if(list != NULL){
		tmp = list;
		do{
			elemNext = tmp->next;
			if(freeStr != 0 && tmp->type == STRING){
				free((char*)tmp->val);
			}
			free(tmp);
			tmp = elemNext;
		}while(tmp != NULL);
	}
	return NULL;
}

linkedlist_t mergeL(linkedlist_t list1, linkedlist_t list2){
	element* tmp;
	
	if(list1 == NULL){
		return list2;
	}
	for(tmp = list1; tmp->next != NULL; tmp = tmp->next);
	tmp->next = list2;
	return list1;
}



int nbOccTypeL(linkedlist_t list, Type_e type){
	element* tmp;
	int nb = 0;
	if(list == NULL){
		return 0;
	}
	for(tmp = list; tmp != NULL; tmp = tmp->next){
		if(tmp->type == type){
			nb++;
		}
	}
	return nb;
}

char* listToStringL(linkedlist_t list){
	element* tmp;
	char buff[10];
	char* strRet = NULL;
	int size = 0; // Taille de la chaine
	if(list == NULL){
		return NULL;
	}
	for(tmp = list; tmp != NULL; tmp = tmp->next){
		if(tmp->type == INT){
			size += sprintf(buff, "%d", (int)tmp->val);
		}
		else if(tmp->type == STRING){
			size += strlen((char*)tmp->val);
		}
	}
	
	if(size == 0){
		return NULL;
	}
	size++;
	strRet = malloc(size * sizeof(char));
	*strRet = '\0'; // Pour que le strcat se face dès le début
	for(tmp = list; tmp != NULL; tmp = tmp->next){
		if(tmp->type == INT){
			sprintf(buff, "%d", (int)tmp->val);
			strcat(strRet, buff);
		}
		else if(tmp->type == STRING){
			strcat(strRet, (char*)tmp->val);
		}
	}
	*(strRet + size-1) = '\0';
	printf("StrRet %s size : %d et len %d\n", strRet, size, strlen(strRet));
	
	return strRet;
}


void** findL(linkedlist_t list, int num){
	element* tmp;
	pair_t pair;
	for(tmp = list; tmp != NULL; tmp = tmp->next){
		if(tmp->type == PAIR){
			pair = *(pair_t*)tmp->val;
			if(pair.num == num){
				return pair.val;
			}
		}
	}
	return NULL;
}


linkedlist_t addPairL(linkedlist_t list, void* val, int num){
	pair_t* pair = malloc(sizeof(pair_t));
	pair->num = num;
	pair->val = val;
	return addQueueL(list, pair, PAIR);
}


