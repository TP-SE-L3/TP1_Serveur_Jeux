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
