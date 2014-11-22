#ifndef COMMANDS_H
#define COMMANDS_H

#include "linkedlist.h"

/*Peremet de faire attendre le client pendant un nombre de secondes*/
void waitc(char** listCommand, int time);

void outc(char** listCommand, char* format, ...);
void inc(char** listCommand, char* format);

/* Permet d'exécuter une commande système sur le client
 * @param listCommand : la liste des commande (liste de chaine)
 * @param commandSys : Chaine contenant la command système*/
void systemc(char** listCommand, char* commandSys);


/* Permet de rajouter une commande out à la liste des commandes
 * @param listCommand: la liste des commande (liste de chaines)
 * @param format: le format dans lequel on va mettre les arguments de la commande(!!Mettre de espace entre les éléments du format)
 * @param ... : Touts les arguments de la commande*/
void outc2(linkedlist_t *listCommand, char* format, ...);

/* Permet de rajouter une commande in à la liste des commandes
 * @param listCommand: la liste des commande (liste de chaines)
 * @param format: Le format de la commande : "%n" avec n € {'s','d'}*/
void inc2(linkedlist_t *listCommand, char* format);

#endif
