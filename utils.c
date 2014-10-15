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

char* substrpbrk(char* str, char* end){
	int i=0;
	for(i=0; *(str+i) != *end && *(str+i) != '\0'; i++);
	if(*(str+i) == '\0'){
		return NULL;
	}
	*(str+i) = '\0';
	return (str+i+1);
}
