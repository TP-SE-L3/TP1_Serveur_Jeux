#ifndef CORE_SERV_H
#define CORE_SERV_H

#include "../linkedlist.h"
#include "../core.h"


typedef struct player_t player_t;
struct player_t{
	int id;
	SOCKET sock; // Le numéro de socket correspondant à ce client
};

typedef struct game_t game_t;
struct game_t{
	int id;
	int idTypeGame;
	int nbPlayer;
	player_t player[2]; // La liste des joueurs
	int pipeWR[2];
	int pipeRW[2];
};



#define GAME_PENDU 1
#define GAME_MORPION 2
#define CHOICE_EXIT 3

/* Permet de gérer les jeux en fonctions des demandes client
 * Retourne les commandes à envoyer au client sous forme d'une chaine de caractère
 * @param idGame: l'id du jeux actuellement joué (Peut être modifié)
 * @param idCli: l'id du client
 * @param args: les paramètre envoyés par le client (Réponses du client)
 * */
char* gameManager(int* idGame, player_t client, linkedlist_t args, linkedlist_t* listGames, int *currentIndex);

/* Converti un chaine en liste d'arguments
 * Retourne une liste d'éléments récupérer dans la réponse client(char*)
 * @param char* resp: La réponse du client*/
linkedlist_t getRespCli(char* resp);

/* Permet de lancer une partie -> Conçus pour être appelée par un thread
 * @param arg: Doit contenir les informations sur la partie sous forme d'une structure game_t
 * */
void* playGame(void* arg);
#endif
