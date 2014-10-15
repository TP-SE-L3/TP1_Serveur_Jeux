#ifndef PARSER_H
#define PARSER_H

#define TAG_COMMAND "cmd:"

typedef struct command_t command_t;
struct command_t{
	char* name;
	char* strArgs;
};


/* Retourne le nom da la première commande trouvée ainsi sa chaine d'arguemnt si elle en a
 * Si aucune commande n'est trouvé, la fonction retourne NULL
 * 
*/
command_t getCommand(char* str);

#endif
