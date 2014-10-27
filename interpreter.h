#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parser.h"

/* Interprète une commande client
 * Retourn -1 si la commande n'a pas put être interprétée
 * @param command: La commande à interpréter */
int performCommandCli(command_t command);


#endif
