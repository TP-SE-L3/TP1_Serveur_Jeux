#ifndef CORE_SERV_H
#define CORE_SERV_H

#include "../linkedlist.h"

typedef struct game_t game_t;
struct game_t{
	int id;
	int idTypeGame;
	int nbPlayer;
	int pipeW;
	int pipeR;
};

#define SIZE_LIST_GAME 20

#define GAME_PENDU 1
#define GAME_MORPION 2
#define CHOICE_EXIT 3

/* Permet de gérer les jeux en fonctions des demandes client
 * Retourne les commandes à envoyer au client sous forme d'une chaine de caractère
 * @param idGame: l'id du jeux actuellement joué (Peut être modifié)
 * @param idCli: l'id du client
 * @param args: les paramètre envoyés par le client (Réponses du client)
 * */
char* gameManager(int* idGame, int idCli, linkedlist_t args, linkedlist_t* listGames, int *currentIndex);

/* Converti un chaine en liste d'arguments
 * Retourne une liste d'éléments récupérer dans la réponse client(char*)
 * @param char* resp: La réponse du client*/
linkedlist_t getRespCli(char* resp);

#endif
