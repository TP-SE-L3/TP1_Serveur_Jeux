#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "../../comands.h"
#include "../../parser.h"

#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif

void init_out_players(char* tabPl[], int nbPlayers);
void close_out_players(char* tabPl[], int nbPlayers);
void flushc(int fd, char* tabPl[], int nbPlayers);

#define NB_PLAYER 2

int main(int argc, char** argv){
	char* outputPl[NB_PLAYER] = {NULL, NULL};
	int fd;
	if(argc >= 1){
		fd = atoi(argv[0]);
		printf("------ Pipe arr : %d ----------\n", fd);
	}
	else{
		perror("Erreur, arguments incorrects.\n");
		exit(EXIT_FAILURE);
	}
	
	
	outc(&outputPl[0], "%s", "Bonjour");
	inc(&outputPl[0], "%s");
	outc(&outputPl[1], "%s", "Salut");
	
	//~ printf("respJ1 : %s\n", outputPl[0]);
	//~ printf("respJ2 : %s\n", outputPl[1]);
	flushc(fd, outputPl, NB_PLAYER);
	
	
	sleep(10);
	
	close(fd);
	close_out_players(outputPl, NB_PLAYER);
	return EXIT_SUCCESS;
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
			//~ header[HEADER_PIPE_SIZE] = '\0';
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
