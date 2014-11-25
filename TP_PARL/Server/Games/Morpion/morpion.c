#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#include "../../../comands.h"
#include "../../../parser.h"
#include "../../../linkedlist.h"

#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif
#define SIZE_BUF 512


void init_out_players(char* tabPl[], int nbPlayers);
void close_out_players(char* tabPl[], int nbPlayers);
void flushc(int fd, char* tabPl[], int nbPlayers);
void* getResp(linkedlist_t* listResp, Type_e type);
linkedlist_t respToList(char* resp);
void reversePlayer(int* j1, int* j2);
int nextPlyer(int J);
void quit(int fd); // Signale au serveur que l'on quitte

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


















int nextPlyer(int J){
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
