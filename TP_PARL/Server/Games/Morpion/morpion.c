#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


#define NB_PLAYER 2

#include "../coreGames.h"

#define HAUTEUR 3
#define LARGEUR 3

void dessinerPlateau(char** out, char plateau[HAUTEUR][LARGEUR]);
void init_plateau(char pl[HAUTEUR][LARGEUR]);
int jouer(char pl[HAUTEUR][LARGEUR], int cel, char car);
int gagner(char pl[HAUTEUR][LARGEUR], char car);
int estPlein(char pl[HAUTEUR][LARGEUR]);

int main(int argc, char** argv){
	char* outPl[NB_PLAYER] = {NULL, NULL}; // Les sorties pour chaque joueur
	linkedlist_t listResp = NULL;
	int pipeW;
	int J1 = 0;
	int J2 = 1;
	int i;
	int jGagne = -1;
	int jPerdu;
	int plein = 0;
	int choix;
	char caract[NB_PLAYER];
	caract[0] = 'X';
	caract[1] = 'O';
	
	char plateau[HAUTEUR][LARGEUR];
	
	if(argc >= 1){
		pipeW = atoi(argv[0]);
	}
	else{
		perror("Erreur, arguments incorrects.\n");
		exit(EXIT_FAILURE);
	}
	
	init_plateau(plateau);


	
	
	while(jGagne == -1 && !plein){
		for(i=0; i < NB_PLAYER; i++){
			systemc(&outPl[J1], "clear");
			outc(&outPl[J1], "%s", "======= Morpion =======\n\n");
			dessinerPlateau(&outPl[J1], plateau);
			outc(&outPl[J1], "%s", "Entrez un numero de case (1, 2, ...) : ");
			inc(&outPl[J1], "%d");
			
			systemc(&outPl[J2], "clear");
			outc(&outPl[J2], "%s", "======= Morpion =======\n\n");
			dessinerPlateau(&outPl[J2], plateau);
			outc(&outPl[J2], "%s", "C'est a votre adversaire de jouer.\n");
			
			flushc(pipeW, outPl, NB_PLAYER);
			choix = (int)getResp(&listResp, INT);
			if(jouer(plateau, choix, caract[J1])){
				if(gagner(plateau, caract[J1])){
					jGagne = J1;
					break;
				}
				if(estPlein(plateau)){
					plein = 1;
					break;
				}
			}
			reversePlayer(&J1, &J2);
		}
	}
	
	systemc(&outPl[J1], "clear");
	systemc(&outPl[J2], "clear");
	if(!plein && jGagne != -1){
		jPerdu = nextPlayer(jGagne);
		outc(&outPl[jGagne], "%s", "======= Morpion =======\n\n");
		outc(&outPl[jGagne], "%s", "Vous avez gagne, felicitation !!\n");
		
		outc(&outPl[jPerdu], "%s", "=======  Morpion =======\n\n");
		outc(&outPl[jPerdu], "%s", "Vous avez perdu, votre adversaire etait peut etre plus intelligent...\n");
	}
	else{
		outc(&outPl[J1], "%s", "=======  Morpion =======\n\n");
		outc(&outPl[J1], "%s", "Vous etes aussi fort (ou mauvais) que votre adversaire...\n");
		outc(&outPl[J2], "%s", "=======  Morpion =======\n\n");
		outc(&outPl[J2], "%s", "Vous etes aussi fort (ou mauvais) que votre adversaire...\n");
	}
	flushc(pipeW, outPl, NB_PLAYER);
	
	// On attend un peut avant de quitter
	waitc(&outPl[J1], 3);
	waitc(&outPl[J2], 3);
	quitc(&outPl[J1]); // Fermera le jeu pour le client
	quitc(&outPl[J2]);
	flushc(pipeW, outPl, NB_PLAYER);
	quit(pipeW);
	close(pipeW);
	close_out_players(outPl, NB_PLAYER);
	return EXIT_SUCCESS;
}


void dessinerPlateau(char** out, char plateau[HAUTEUR][LARGEUR]){
	outc(out, "%s", " ┌───┬───┬───┐\n");
	outc(out, "%s %c %s %c %s %c %s"," │ ", plateau[0][0], " │ ", plateau[0][1], " │ ", plateau[0][2], " │\n ├───┼───┼───┤\n");
	outc(out, "%s %c %s %c %s %c %s"," │ ", plateau[1][0], " │ ", plateau[1][1], " │ ", plateau[1][2], " │\n ├───┼───┼───┤\n");
	outc(out, "%s %c %s %c %s %c %s"," │ ", plateau[2][0], " │ ", plateau[2][1], " │ ", plateau[2][2], " │\n └───┴───┴───┘\n");
}

void init_plateau(char pl[HAUTEUR][LARGEUR]){
	int i, j;
	for(i=0; i < HAUTEUR; i++){
		for(j=0; j < LARGEUR; j++){
			pl[i][j] = ' ';
		}
	}
}

int jouer(char pl[HAUTEUR][LARGEUR], int cel, char car){
	int i, j, cpt=0;
	for(i=0; i < 3; i++){
		for(j=0; j < 3; j++){
			cpt++;
			if(cpt == cel){
				if(pl[i][j] == ' '){
					pl[i][j] = car;
					return 1;
				}
				else{
					return 0;
				}
			}
		}
	}
	return 0;
}

int estPlein(char pl[HAUTEUR][LARGEUR]){
	int i, j;
	for(i=0; i < HAUTEUR; i++){
		for(j=0; j < LARGEUR; j++){
			if(pl[i][j] == ' '){
				return 0;
			}
		}
	}
	return 1;
}

int gagner(char pl[HAUTEUR][LARGEUR], char car){
	int i;
	for(i=0; i < 3; i++){
		if(pl[i][0] == car && pl[i][1] == car && pl[i][2] == car){
			return 1;
		}
	}
	for(i=0; i < 3; i++){
		if(pl[0][i] == car && pl[1][i] == car && pl[2][i] == car){
			return 1;
		}
	}
	if(pl[0][0] == car && pl[1][1] == car && pl[2][2] == car){
		return 1;
	}
	if(pl[2][0] == car && pl[1][1] == car && pl[0][2] == car){
		return 1;
	}
	return 0;
}
