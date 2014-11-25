#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>



#include "coreGames.h"


int nextPlayer(int J){
	if((J+1) >= NB_PLAYER){
		return 0;
	}
	return J+1;
}

// Inverse les numéros des joueurs
void reversePlayer(int* j1, int* j2){
	int tmp = *j1;
	*j1 = *j2;
	*j2 = tmp;
}

void flushc(int fd, char* tabPl[], int nbPlayers){
	int i, j;
	char header[HEADER_PIPE_SIZE+1];
	int waitRep;
	
	for(i=0; i < nbPlayers; i++){
		waitRep = FALSE;
		if(tabPl[i] != NULL){
			if(strstr(tabPl[i], "cmd:in ") != NULL){
				waitRep = TRUE; // On attend un réponse du client
			}
			sprintf(header, "[PL:%d RP:%d SIZE:%d];", i, waitRep, strlen(tabPl[i]));
			for(j = strlen(header); j < HEADER_PIPE_SIZE; j++){
				header[j] = ' '; // On comble la taille qui manque avec des espaces
			}
			write(fd, header, HEADER_PIPE_SIZE);
			write(fd, tabPl[i], strlen(tabPl[i]));
			free(tabPl[i]);
			tabPl[i] = NULL;
		}
	}
}


void close_out_players(char* tabPl[], int nbPlayers){
	int i;
	// Libère juste la mémoire des chaines
	for(i=0; i < nbPlayers; i++){
		if(tabPl[i] != NULL){
			free(tabPl[i]);
		}
	}
}


void* getResp(linkedlist_t* listResp, Type_e type){
	char buf[SIZE_BUF];
	void* val = NULL;
	if(isEmptyL(*listResp)){
		read(STDIN_FILENO, buf, SIZE_BUF);
		*listResp = respToList(buf);
	}
	element* el = *listResp;
	element* elPre = NULL; // élément précédent (pour la suppression)
	for(el = *listResp; el != NULL; el = el->next){
		if(el->type == type){ // Cherche un élément du type demandé
			if(elPre == NULL){
				*listResp = NULL;
			}
			else{
				elPre->next = el->next; // Enlève ce maillon de la chaîne
			}
			break;
		}
	}
	if(el == NULL){
		perror("Erreur dans la fonction de recuperation des reponses.");
		exit(EXIT_FAILURE);
	}
	val = el->val;
	free(el);
	return val;
}

linkedlist_t respToList(char* resp){
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
			list = addHeadL(list, (void*)atoi(arg), INT);
			free(arg);
		}
		else{ // Ajoute un float
			float* respFloat = malloc(sizeof(float));
			*respFloat = (float)atof(arg);
			list = addHeadL(list, respFloat, FLOAT);
			free(arg);
		}
	}
	return list;
}


void quit(int fd){
	char header[HEADER_PIPE_SIZE];
	int i;
	sprintf(header, "[PL:%d RP:%d SIZE:%d];", 0, 0, -1); // Taille -1 signifie qu'on quitte
	for(i = strlen(header); i < HEADER_PIPE_SIZE; i++){
		header[i] = ' '; // On comble la taille qui manque avec des espaces
	}
	write(fd, header, HEADER_PIPE_SIZE);
	sleep(1); // Attend une seconde
}

//~ void init_out_players(char* tabPl[], int nbPlayers){
	//~ int i;
	//~ char start[] = "<playerN rep=0>";
	//~ for(i = 0; i < nbPlayers; i++){
		//~ tabPl[i] = NULL;
		//~ start[7] = '1' + (char)i; // Met le bon numéro du joueur
		//~ tabPl[i] = malloc(strlen(start)+1);
		//~ strcpy(tabPl[i], start);
	//~ }
	//~ 
//~ }

//~ void flushc(int fd, char* tabPl[], int nbPlayers){
	//~ char end[] = "</playerN>";
	//~ int i;
	//~ for(i = 0; i < nbPlayers; i++){
		//~ if(strstr(tabPl[i], "cmd:in ") != NULL){
			//~ tabPl[i][13] = '1'; // <playerN rep=1...
		//~ }
		//~ end[8] = '1' + (char)i; // Met le bon numéro du joueur
		//~ write(fd, tabPl[i], strlen(tabPl[i]));
		//~ write(fd, end, strlen(end));
		//~ free(tabPl[i]);
	//~ }
	//~ 
	//~ init_out_players(tabPl, nbPlayers);
//~ }
