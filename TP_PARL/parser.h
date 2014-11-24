#ifndef PARSER_H
#define PARSER_H
#include "linkedlist.h"

#define TAG_COMMAND "cmd:"

#define HEADER_PIPE_SIZE 25

// Les types que peut retourner la fonction recupArg

typedef enum TypeArg_e{
	A_ERROR,
	A_STRING,
	A_INT,
	A_FLOAT
} TypeArg_e;


typedef struct command_t command_t;
struct command_t{
	char* name;
	linkedlist_t args;
};

typedef struct headerPipe_t headerPipe_t;
struct headerPipe_t{
	int numPlayer;
	int size;
	int waitRep; // si le serveur doit attendre une réponse
};


/* Retourne le nom da la première commande trouvée ainsi sa chaine d'arguemnt si elle en possède
 * (str est un double pointer, car l'adresse sera déplacée)
 * Si aucune commande n'est trouvé, la fonction retourn une command_t avec un nom null
 * @param str: la chaine dans laquelle on cherche la commande
*/
command_t getCommand(char** str);
/* Permet de récupérer un argument d'une commande
 * Retourne -1 si aucun arguement n'est trouvé
 * si startArg vaut '"' alors l'argument est un chaine
 * sinon l'argument est un nombre
 * @param str: référence sur la chaine dans laquelle on cherche l'arguement(Elle sera modifié)
 * @param startArg: contiendra une valeur de retour qui pointera sur l'adresse du pointer de début
*/
TypeArg_e recupArg(char** str, char** arg);


/* Peremet de formater une réponse en chaîne à partir d'une listechainée
 * Retourne une chaine contenant les éléments de réponse (Attention il faudra vider la mémoire)
 * @param listResp: Liste chainée contenant toutes le éléments de réponse*/
char* formatResponse(linkedlist_t* listResp);


headerPipe_t getHeaderPipe(char* message);
#endif
