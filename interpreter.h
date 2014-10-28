#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parser.h"

/* Interprète une commande client
 * retourne la liste des réponses de commandes (complétée si des commandes l'on modifié)
 * @param command: La commande à interpréter 
 * @param listResp: Ajoute des réponse à la liste si la commande retourne une ou des valeurs */
linkedlist_t performCommandCli(command_t *command, linkedlist_t listResp);



/*---- On utilise l'adresse de args, car en utilisant popL, si la liste devient vide
 * --- Notre liste doit avoir un pointer vide et non pointer sur un élément qui vient d'être libéré*/

/* Peremet d'éxécuter la commande "out" : fait des printf
 * @param args: la liste des arguments de la commande*/
void performOut(linkedlist_t *args);

/* Peremet d'éxécuter la command "in" : fait des scanf
 * Retourne la réponse à la fonction
 * @param args: arguments de la commande 
 * @param listResp: la liste des réponse, qui sera retournée par la fonction*/
linkedlist_t performIn(linkedlist_t* args, linkedlist_t listResp);

#endif
