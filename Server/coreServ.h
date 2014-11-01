#ifndef CORE_SERV_H
#define CORE_SERV_H

#include "../linkedlist.h"

/* Permet de gérer les jeux en fonctions des demandes client
 * Retourne les commandes à envoyer au client sous forme d'une chaine de caractère
 * @param idGame: l'id du jeux actuellement joué (Peut être modifié)
 * @param idCli: l'id du client
 * @param args: les paramètre envoyés par le client (Réponses du client)
 * */
char* gameManager(int* idGame, int idCli, linkedlist_t args);


void outc(char** listCommand, char* format, ...);
void inc(char** listCommand, char* format);

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
