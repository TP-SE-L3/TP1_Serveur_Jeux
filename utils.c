#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

int stroc(char* str1, char* str2){
	int i, j;
	for(i=0; *(str1+i) != '\0'; i++){
		if(*(str1+i) == *str2){
			for(j=0; *(str1+i+j) == *(str2+j) && *(str2+j) != '\0' && *(str1+i+j) != '\0'; j++);
			if(*(str2+j) == '\0'){
				return i; // On a trouvé le mot
			}
		}
	}
	return -1;
}

char* strbtw(char* str, char start, char end){
	int i, j, k;
	char* nwStr;
	for(i=0; *(str+i) != '\0' && *(str+i) != start; i++);
	if(*(str+i) != '\0'){
		i++; // On va plus loin que start
		for(j=i; *(str+j) != '\0' && *(str+j) != end; j++);
		nwStr = malloc((j-i+1) * sizeof(char));
		for(k=i; k < j; k++){// On ne prend pas le caractère end (position j)
			*(nwStr+(k-i)) = *(str+k);
		}
		*(nwStr+k) = '\0';
		return nwStr;
	}
	return NULL;
}

char* substr(char* start, char* end){
	int i=0;
	char* nwStr;
	for(i=0; *(start+i) != *end && *(start+i) != '\0'; i++);
	if(i > 0){
		i = (*(start+i) == '\0')? i-1: i;	
		nwStr = malloc((i+1)*sizeof(char)); //------------ Penser à un realloc start, la taile est obligatoirement plus petite ou égale !!!!
		return nwStr; 						// Tester la nouvelle valeur mémoire de réalloc
	}
	return NULL;
}
