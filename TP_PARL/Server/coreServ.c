#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>

#include "coreServ.h"
#include "../linkedlist.h"
#include "../utils.h"
#include "../parser.h"
#include "../comands.h"
#include "../core.h"
#include "../parser.h"


// Il serait mieux de faire une map
linkedlist_t listTypeGames;


char* gameManager(int* idGame, player_t client, linkedlist_t args, linkedlist_t* listGames, int* currentIndex){
	char* commands = NULL;
	int choix = 0;
	
	
	if(*idGame == -1){
		if(!isEmptyL(args)){
			element* el = popL(&args);
			choix = (int)el->val;
			
			////////////////////////////////////////////////////////////////////////
			////						CHOIX D'UN JEU							////
			////////////////////////////////////////////////////////////////////////
			if(choix <= sizeL(listTypeGames) && choix > 0){ // Si le choix est correct
				element* el;
				for(el = *listGames; el != NULL; el = el->next){
					game_t* curGame = (game_t*)el->val;
					// Si une on rejoint une partie
					if(curGame->idTypeGame == choix && curGame->nbPlayer < 2){
						*idGame = curGame->id;
						curGame->nbPlayer++;
						curGame->player[1] = client;
						
						////////////////////////////////////////////////////////////////////////
						////						THREAD NV PARTIE						////
						////////////////////////////////////////////////////////////////////////
						pthread_t th_Game; // thread pour une partie
 						if(pthread_create(&th_Game, NULL, playGame, curGame)){
							// Si erreur dans la création du thread
							outc(&commands, "%s", "Impossible de lancer la partie en parallele.");
							printf("Erreur pthread game : %d\n", curGame->id);
							// Ferme le socket du premier client (Pas idéale, il faudrai d'abord prévenir le client)
							if(shutdown(curGame->player[0].sock, SHUT_RDWR) == -1){
								perror("Error to shutdown sockets");
								exit(EXIT_FAILURE);
							}
							close(curGame->player[0].sock);
							*idGame = -1;
						}
 						//waitc(&commands, 5); // On attend pour le moment		
						break;
					}
				}
				
				if(*idGame == -1){
					game_t *nwGame = malloc(sizeof(game_t)); // Le pointeur à enregistrer dans la liste
					nwGame->id = *currentIndex;
					nwGame->idTypeGame = choix;
					nwGame->nbPlayer = 1;
					nwGame->player[0] = client;
					
					*listGames = addHeadL(*listGames, nwGame, OTHER);
					*currentIndex += 1;
					*idGame = nwGame->id;
					outc(&commands, "%s", "En attente d'un autre joueur.\n");
					//waitc(&commands, 5);
				}
			}
			
			////////////////////////////////////////////////////////////////////////
			////							QUITTER								////
			////////////////////////////////////////////////////////////////////////
			else if(choix == sizeL(listTypeGames)+1){
				outc(&commands, "%s", "Au revoir...\n");
				quitc(&commands);
			}
			else{
				outc(&commands, "%s", "Choix incorrect...\n\n"); 
				waitc(&commands, 1);
				choix = 0; 
			}
		}
		////////////////////////////////////////////////////////////////////////
		////							MENU								////
		////////////////////////////////////////////////////////////////////////
		if(choix == 0){
			
			systemc(&commands, "clear");
			outc(&commands, "%s", "=================== MENU ===================\n");
			// Affiche la liste des jeux possibles
			element* elem;
			typeGame_t curTypeG;
			for(elem = listTypeGames; elem != NULL; elem = elem->next){
				curTypeG = *(typeGame_t*)(*(pair_t*)elem->val).val; // Récupère le typeGame dans la paire
				outc(&commands, "%d %s %s %s", curTypeG.id, ". ", curTypeG.name, "\n");
			}
			outc(&commands, "%d %s", sizeL(listTypeGames)+1, ". Quitter\n"); // Quitter est le dernier le nombre de jeux+1
			inc(&commands, "%d");
		}
	}
	else{ // Si idGame != -1 Càd si le joueur est déjà dans une partie
		////////////////////////////////////////////////////////////////////////
		////					Parties en cours (ou attente)				////
		////////////////////////////////////////////////////////////////////////
		element* el;
		game_t curGame;
		for(el = *listGames; el != NULL; el = el->next){
			curGame = *(game_t*)el->val;
			if(curGame.id == *idGame){
				break;
			}
		}
		if(el == NULL){
			outc(&commands, "%s", "Erreur : Impossible de trouver la partie.\n");
			*idGame = -1;
		}
		else{
			if(curGame.nbPlayer < 2){
				outc(&commands, "%s", "."); // On attend toujours un autre joueur
				//waitc(&commands, 5);
			}
			else{
				outc(&commands, "%s", "La partie peut commencer...\n");
				//waitc(&commands, 15); // On attends pour le moment
			}
		}
	}
	
	
	/*commands = listToStringL(listCommand);
	listCommand = cleanL(listCommand, 1); // Vide la liste*/
	
	return commands;
}



void initListTypeGames(FILE* file){
	typeGame_t* nwtypeG = NULL;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	while ((read = getline(&line, &len, file)) != -1) {
		nwtypeG = malloc(sizeof(typeGame_t));
		printf("%s", line);
		getGameType(line, nwtypeG);
		listTypeGames = addPairL(listTypeGames, nwtypeG, nwtypeG->id);
	}
	
	
	/*typeGame_t* nwtypeG1 = malloc(sizeof(typeGame_t));
	typeGame_t* nwtypeG2 = malloc(sizeof(typeGame_t));
	nwtypeG1->id = 1;
	strcpy(nwtypeG1->name, "Pendu");
	strcpy(nwtypeG1->path, "Games/Pendu/Pendu");
	nwtypeG1->nbJoueur = 2;
	
	nwtypeG2->id = 2;
	strcpy(nwtypeG2->name, "Morpion");
	strcpy(nwtypeG2->path, "Games/Morpion/Morpion");
	nwtypeG2->nbJoueur = 2;
	listTypeGames = addPairL(listTypeGames, nwtypeG2, nwtypeG2->id);*/
}


void getGameType(char* string, typeGame_t* gameType){
	char* strCurrent; // Chaine qui va être modifée, petit à petit pour récupérer les bon éléments
	char* startStrCurrent; // Le pointeur de départ de strCurrent, pour libérer la mémoire à la fin
	char* idStr;
	char* resStr; // Tmp variable de résultat
	
	
	// Revoir le nom des variables
	// Les paramètres doivent être dans le bon ordre
	strCurrent = strbtw(string, '[', ']');
	startStrCurrent = strCurrent;
	resStr = strstr(strCurrent, "ID:"); // Récupère l'id du joueur
	if(resStr != NULL){
		strCurrent = resStr;
		idStr = strCurrent+strlen("ID:");
		strCurrent = substrpbrk(idStr, " \0");
		gameType->id = atoi(idStr);
	}
	resStr = strstr(strCurrent, "NAME:"); 
	if(resStr != NULL){
		strCurrent = resStr;
		idStr = strCurrent+strlen("NAME:");
		strCurrent = substrpbrk(idStr, " \0");
		strcpy(gameType->name, idStr);
	}
	resStr = strstr(strCurrent, "PATH:"); 
	if(resStr != NULL){
		strCurrent = resStr;
		idStr = strCurrent+strlen("PATH:");
		strCurrent = substrpbrk(idStr, " \0");
		strcpy(gameType->path, idStr);
	}
	resStr = strstr(strCurrent, "NB_PL:"); // Récupère l'id du joueur
	if(resStr != NULL){
		strCurrent = resStr;
		idStr = strCurrent+strlen("NB_PL:");
		strCurrent = substrpbrk(idStr, " \0");
		gameType->nbJoueur = atoi(idStr);
	}
	free(startStrCurrent);
}




linkedlist_t getRespCli(char* resp){
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


void* playGame(void* arg){
	char* message = NULL;
	header_t header = {-1, -1, -1};
	game_t* game = (game_t*)arg;
	headerPipe_t headerPipe[2]; // ----- Demander d'où vient le prblème si headerPipe n'est pas initilisé avant le fork
	pid_t pid;
	// On récupère les informations sur le jeu
	typeGame_t gameType = *(typeGame_t*)findL(listTypeGames, game->idTypeGame);
	int quit = 0; // Est mit à 1 quand on doit quitter la partie
	
	
	printf(" [ID:%d | Socket:%d | Partie:%d -> %s]\n", game->player[0].id, game->player[0].sock, game->id, gameType.name);
	printf(" [ID:%d | Socket:%d | Partie:%d -> %s]\n", game->player[1].id, game->player[1].sock, game->id, gameType.name);
	
	if(pipe(game->pipeR) == -1){
		perror("Erreur tube RW");
		exit(EXIT_FAILURE);
	}
	if(pipe(game->pipeW) == -1){
		perror("Erreur tube WR");
		exit(EXIT_FAILURE);
	}
	
	pid = fork();
	if(pid < 0){
		perror("Erreur lors du fork sur le jeu.");
		exit(EXIT_FAILURE);
	}
	if(pid == 0){ // On est dans le fils
		close(game->pipeR[0]); // Ferme la lecture
		close(game->pipeW[1]);
		char strpipeR[10];
		sprintf(strpipeR, "%d", game->pipeR[1]);
		char* argList[] = {strpipeR};
		dup2(game->pipeW[0], STDIN_FILENO); // pipeW remplace le fichier d'entrée stdin
		close(game->pipeW[0]);
		
		execvp(gameType.path, argList);
		printf("Erreur impossible de lancer le jeu.\n");
		exit(EXIT_FAILURE);
	}
	else{
		close(game->pipeR[1]); // Ferme l'écriture
		close(game->pipeW[0]); // Ferme la lecture
		char headerPipeStr[HEADER_PIPE_SIZE+1];
		char* msgToSend = NULL; // Récupère les commandes clients dans le pipe
		int i;
		
		header.idGame = game->id;
		while(!quit){
			////////////////////////////////////////////////////////////////////////
			////							ENVOIES CLIENTS						////
			////////////////////////////////////////////////////////////////////////
			// i n'est pas forcément le numéro du joueur, le numéro du joueur est dans headerPipe
			// On envoie toutes les commandes qu'il faut envoyer à chaque client
			for(i = 0; i < game->nbPlayer; i++){
				read(game->pipeR[0], headerPipeStr, HEADER_PIPE_SIZE);
				headerPipeStr[HEADER_PIPE_SIZE] = '\0';
				
				//printf("Pipe : %s\n", headerPipeStr);
				headerPipe[i] = getHeaderPipe(headerPipeStr);
				if(headerPipe[i].size == -1){ // On quitter la partie ------------- 
					quit = 1;
					break;
				}
				
				if(headerPipe[i].size != 0){
					msgToSend = malloc(headerPipe[i].size + 1);
					read(game->pipeR[0], msgToSend, headerPipe[i].size);
					msgToSend[headerPipe[i].size] = '\0';
					header.size = ((msgToSend == NULL)? (-1) : (signed int)strlen(msgToSend));
					header.id = game->player[headerPipe[i].numPlayer].id;
					sendHeader(game->player[headerPipe[i].numPlayer].sock, header);
					if(msgToSend != NULL){
						sendMessage(game->player[headerPipe[i].numPlayer].sock, msgToSend);
						free(msgToSend);
						msgToSend = NULL;
					}
				}
			}
			
			////////////////////////////////////////////////////////////////////////
			////						RECEPTION CLIENTS						////
			////////////////////////////////////////////////////////////////////////
			
			for(i=0; i < game->nbPlayer; i++){
				// Si on attend un réponse ce client (client i)
				if(headerPipe[i].waitRep == 1){
					SOCKET sockCli = game->player[headerPipe[i].numPlayer].sock;
					if(recvHeader(sockCli, &header) == -1){
						perror("Erreur dans la réception de l'entete d'un client");
						exit(EXIT_FAILURE);
					}
					
					if(header.size != -1){ // Si -1, aucun msg à recevoir
						message = malloc(header.size);
						int res = recv(sockCli, message, header.size, 0);
						message[header.size-1] = '\0'; // Met le caractère '\0' en fin de chaine
						write(game->pipeW[1], message, strlen(message));
						if(res != header.size-1){
							perror("Erreur de reception");
							exit(EXIT_FAILURE);
						}
						if(res == -1){
							perror("Error recv");
							exit(EXIT_FAILURE);
						}
						free(message);
					}
				}
			}
		}
		
		for(i=0; i < game->nbPlayer; i++){
			if(shutdown(game->player[i].sock, SHUT_RDWR) == -1){
				perror("Error to shutdown sockets");
				exit(EXIT_FAILURE);
			}
			close(game->player[i].sock);
		}
		close(game->pipeR[0]); // Fermer pipe lecture
		close(game->pipeW[1]); // Fermer pipe écriture
	}
	
	return arg;
}
