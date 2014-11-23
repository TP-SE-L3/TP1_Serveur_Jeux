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


char* gameManager(int* idGame, player_t client, linkedlist_t args, linkedlist_t* listGames, int* currentIndex){
	char* commands = NULL;
	int choix = 0;
	
	if(*idGame == -1){
		if(!isEmptyL(args)){
			element* el = popL(&args);
			choix = (int)el->val;
			switch(choix){
				case GAME_PENDU:
					outc(&commands, "%s", "Pendu\n");
				break;
				case GAME_MORPION:
					outc(&commands, "%s", "Morpion\n");
				break;
				case CHOICE_EXIT:
					outc(&commands, "%s", "Quitter\n");
				break;
				default: 
					outc(&commands, "%s", "Choix incorrect...\n\n"); 
					waitc(&commands, 1);
					choix = 0; 
				break;
			}
			if(choix != 0 && choix != CHOICE_EXIT){
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
		}
		////////////////////////////////////////////////////////////////////////
		////							MENU								////
		////////////////////////////////////////////////////////////////////////
		if(choix == 0){
			systemc(&commands, "clear");
			outc(&commands, "%s", "=================== MENU ===================\n");
			outc(&commands, "%d %s", GAME_PENDU, ". Pendu\n");
			outc(&commands, "%d %s", GAME_MORPION, ". Morpion\n");
			outc(&commands, "%d %s", CHOICE_EXIT, ". Quitter\n");
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
	//char* message;
	header_t header = {-1, -1, -1};
	
	//~ linkedlist_t respCli = NULL;
	char* msgToSend = NULL;
	game_t* game = (game_t*)arg;
	pid_t pid;
	
	if(pipe(game->pipeRW) == -1){
		perror("Erreur tube RW");
		exit(EXIT_FAILURE);
	}
	
	pid = fork();
	if(pid < 0){
		perror("Erreur lors du fork sur le jeu.");
		exit(EXIT_FAILURE);
	}
	if(pid == 0){ // On est dans le fils
		close(game->pipeRW[0]); // Ferme la lecture
		char strPipeRW[10];
		sprintf(strPipeRW, "%d", game->pipeRW[1]);
		char* argList[] = {strPipeRW};
		printf("Pipe : %d\n", game->pipeRW[1]);
		
		execvp("Games/Pendu", argList);
		printf("Errorrrrrrrrrrrrr");
	}
	else{
		close(game->pipeRW[1]); // Ferme l'écriture
		char headerPipe[26];
		read(game->pipeRW[0], headerPipe, 25);
		headerPipe[25] = '\0';
		char cmdPipe[37];
		read(game->pipeRW[0], cmdPipe, 36);
		cmdPipe[36] = '\0';
		printf("Msg pipe : %s\n", cmdPipe); 
		
		
		
		//~ printf("THREAD !! et sock Client 1 : %d\n", game->player[0].sock);
		//~ outc(&msgToSend, "%s", "1 : La partie peut commencer...\n");
		//~ header.size = ((msgToSend == NULL)? (-1) : (signed int)strlen(msgToSend));
		//~ sendHeader(game->player[0].sock, header);
		//~ if(msgToSend != NULL){
			//~ header.size = strlen(msgToSend)+1;
			//~ sendMessage(game->player[0].sock, msgToSend);
			//~ free(msgToSend);
			//~ msgToSend = NULL;
		//~ }
		//~ 
		//~ outc(&msgToSend, "%s", " 2 : La partie peut commencer...\n");
		//~ header.size = ((msgToSend == NULL)? (-1) : (signed int)strlen(msgToSend));
		//~ sendHeader(game->player[1].sock, header);
		//~ if(msgToSend != NULL){
			//~ header.size = strlen(msgToSend)+1;
			//~ sendMessage(game->player[1].sock, msgToSend);
			//~ free(msgToSend);
			//~ msgToSend = NULL;
		//~ }
		
		/*if(recvHeader(sockCli, &header) == -1){
			perror("Erreur dans la réception de l'entete d'un client");
			exit(EXIT_FAILURE);
		}
		
		if(header.size != -1){ // Si -1, aucun msg à recevoir
			message = malloc(header.size);
			int res = recv(sockCli, message, header.size, 0);
			message[header.size-1] = '\0'; // Met le caractère '\0' en fin de chaine
			if(res != header.size-1){
				perror("Erreur de reception");
				exit(EXIT_FAILURE);
			}
			
			if(res == -1){
				perror("Error recv");
				exit(EXIT_FAILURE);
			}
			respCli = getRespCli(message); // Récupère les réponses client
			
			free(message);
		}
		player_t playerCli = {header.id, sockCli}; // Informations sur le client
		int idGameBefore = header.idGame;
		msgToSend = gameManager(&header.idGame, playerCli, respCli, &listGames, &indiceListGame);
		// Nouvelle partie, on est entré dans un thread ou on attend un autre joueur
		if(idGameBefore != header.idGame && header.idGame >= 0){
			closeSock = FALSE; // On ne ferme pas la socket
		}
		respCli = cleanL(respCli, 0);*/
		
		
		
		
		//~ if(shutdown(game->player[0].sock, SHUT_RDWR) == -1){
			//~ perror("Error to shutdown sockets");
			//~ exit(EXIT_FAILURE);
		//~ }
		//~ close(game->player[0].sock);
		//~ 
		//~ if(shutdown(game->player[1].sock, SHUT_RDWR) == -1){
			//~ perror("Error to shutdown sockets");
			//~ exit(EXIT_FAILURE);
		//~ }
		//~ close(game->player[1].sock);
	}
	
	return arg;
}
