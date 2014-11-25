#ifndef CORE_GAMES
#define CORE_GAMES

#include "../../comands.h"
#include "../../parser.h"
#include "../../linkedlist.h"

#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif
#define SIZE_BUF 512

#ifndef NB_PLAYER
#define NB_PLAYER 2
#endif

////////////////////////////////////////////////////////////////////////////////////
////	Contient des fonctions utiles pour créer des jeux sur le serveur		////
////////////////////////////////////////////////////////////////////////////////////

void init_out_players(char* tabPl[], int nbPlayers);
void close_out_players(char* tabPl[], int nbPlayers);
void flushc(int fd, char* tabPl[], int nbPlayers);
void* getResp(linkedlist_t* listResp, Type_e type);
linkedlist_t respToList(char* resp);
void reversePlayer(int* j1, int* j2);
int nextPlayer(int J);
void quit(int fd); // Signale au serveur que l'on quitte

#endif
