#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>


#define NB_PLAYER 2

#include "../coreGames.h"

char* initHiddenWord(char* word);
int searchInHidden(char* hidWord, char* word, char car);
void upperWord(char* word);

#define NB_PLAYER 2

int main(int argc, char** argv){
	char* outPl[NB_PLAYER] = {NULL, NULL}; // Les sorties pour chaque joueur
	linkedlist_t listResp = NULL;
	int pipeW;
	int J1 = 0;
	int J2 = 1;
	int i;
	
	int nbTry = 0;
	int jGagne = -1; // Le numéro du joueur qui a gagné la partie
	int jPerdu;
	int nbLettre;
	char* word[NB_PLAYER];
	char* hiddenWord[NB_PLAYER];
	char* choice; // Le mot que le joueur a essayer
	
	if(argc >= 1){
		pipeW = atoi(argv[0]);
	}
	else{
		perror("Erreur, arguments incorrects.\n");
		exit(EXIT_FAILURE);
	}
	
	for(i=0; i < NB_PLAYER; i++){
		systemc(&outPl[J1], "clear");
		systemc(&outPl[J2], "clear");
		outc(&outPl[J1], "%s", "======= Jeu du pendu =======\n\n");
		outc(&outPl[J1], "%s", "Entrez un mot(10 carct max): ");
		inc(&outPl[J1], "%s");
		outc(&outPl[J2], "%s", "======= Jeu du pendu =======\n\n");
		outc(&outPl[J2], "%s", "Attendez que l'autre joueur choisisse un mot...\n");
		flushc(pipeW, outPl, NB_PLAYER);
		word[J1] = (char*)getResp(&listResp, STRING);
		upperWord(word[J1]);
		hiddenWord[J1] = initHiddenWord(word[J1]);
		//printf("Les mot choisi est : %s\n", word[J1]);
		reversePlayer(&J1, &J2);
		fflush(stdout);
	}
	
	systemc(&outPl[J2], "clear");
	outc(&outPl[J2], "%s", "======= Jeu du pendu =======\n\n");
	while(jGagne == -1){
		for(i=0; i < NB_PLAYER && jGagne == -1; i++){
			systemc(&outPl[J1], "clear");
			outc(&outPl[J1], "%s", "======= Jeu du pendu =======\n\n");
			outc(&outPl[J1], "%s %d %s", "Nombre d'essais : ", nbTry, "\n");
			outc(&outPl[J1], "%s %s %s", "Le mot a trouver : ", hiddenWord[J2], "\n");
			outc(&outPl[J1], "%s", "C'est a vous, entrez une lettre ou un mot : ");
			inc(&outPl[J1], "%s");
			
			
			outc(&outPl[J2], "%s", "\nC'est a votre adversaire de jouer...\n");
			flushc(pipeW, outPl, NB_PLAYER);
			choice = (char*)getResp(&listResp, STRING);
			if(strlen(choice) > 1){
				upperWord(choice);
				if(strcmp(word[J2], choice) == 0){
					outc(&outPl[J1], "%s", "Votre mot est le bon.\n");
					jGagne = J1;
				}
				else{
					outc(&outPl[J1], "%s", "Votre mot ne correspond pas.\n");
				}
			}
			else{ // Si le joueur a entré un seul caractère
				nbLettre = searchInHidden(hiddenWord[J2], word[J2], choice[0]);
				if(nbLettre == 0){
					outc(&outPl[J1], "%s", "Cette lettre n'est pas presente dans le mot.\n");
				}
				else{
					outc(&outPl[J1], "%s %d %s", "Cette lettre a ete trouve ", nbLettre, " fois dans le mot.\n");
				}
				
				if(strcmp(word[J2], hiddenWord[J2]) == 0){
					jGagne = J1;
				}
			}
			free(choice);
			reversePlayer(&J1, &J2);
		}
		nbTry++;
	}
	
	//printf("Pendu : Le joueur %d a gagne.\n", jGagne+1);
	jPerdu = nextPlyer(jGagne);
	systemc(&outPl[J1], "clear");
	systemc(&outPl[J2], "clear");
	outc(&outPl[jGagne], "%s", "======= Jeu du pendu =======\n\n");
	outc(&outPl[jGagne], "%s %s %s", "Le mot etait bien : ", word[jPerdu], "\n");
	outc(&outPl[jGagne], "%s %d %s", "Vous avez gagne en ", nbTry, " essais, felicitation !!\n");
	
	outc(&outPl[jPerdu], "%s", "======= Jeu du pendu =======\n\n");
	outc(&outPl[jPerdu], "%s", "Vous avez perdu, votre adversaire a ete plus rapide..\n");
	outc(&outPl[jPerdu], "%s %s %s", "Le mot etait : ", word[jGagne],"\n");
	flushc(pipeW, outPl, NB_PLAYER);
	for(i=0; i < NB_PLAYER; i++){
		free(hiddenWord[i]);
		free(word[i]);
	}
	
	// On attend un peut avant de quitter
	waitc(&outPl[J1], 2);
	waitc(&outPl[J2], 2);
	quitc(&outPl[J1]); // Fermera le jeu pour le client
	quitc(&outPl[J2]);
	flushc(pipeW, outPl, NB_PLAYER);
	quit(pipeW);
	close(pipeW);
	close_out_players(outPl, NB_PLAYER);
	return EXIT_SUCCESS;
}

char* initHiddenWord(char* word){
	int i;
	int sizeW = strlen(word);
	char* hidWord = malloc(sizeW+1);
	for(i=0; i < sizeW; i++){
		hidWord[i] = '*';
	}
	hidWord[i] = '\0';
	return hidWord;
}

int searchInHidden(char* hidWord, char* word, char car){
	unsigned int i;
	int nbLettre = 0;
	car = toupper(car);
	for(i=0; i < strlen(word); i++){
		if(toupper(word[i]) == car){
			hidWord[i] = car;
			nbLettre++;
		}
	}
	return nbLettre;
}

void upperWord(char* word){
	unsigned int i;
	for(i=0; i < strlen(word); i++){
		word[i] = toupper(word[i]);
	}
}
