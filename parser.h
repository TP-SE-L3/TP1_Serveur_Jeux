#ifndef PARSER_H
#define PARSER_H

#define TAG_COMMAND "cmd:"

typedef struct command_t command_t;
struct command_t{
	char* name;
	char* strArgs;
};


/* Retourne le nom da la première commande trouvée ainsi sa chaine d'arguemnt si elle en possède
 * Si aucune commande n'est trouvé, la fonction retourn une command_t avec un nom null
 * @param str: la chaine dans laquelle on cherche la commande
*/
command_t getCommand(char* str);
/* Permet de récupérer un argument d'une commande
 * Retourne -1 si aucun arguement n'est trouvé
 * si startArg vaut '"' alors l'argument est un chaine
 * sinon l'argument est un nombre
 * @param str: référence sur la chaine dans laquelle on cherche l'arguement(Elle sera modifié)
 * @param startArg: contiendra une valeur de retour qui pointera sur l'adresse du pointer de début
*/
int recupArg(char** str, char** startArg);

#endif
