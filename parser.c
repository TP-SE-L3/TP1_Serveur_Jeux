#include <string.h>

#include "utils.h"
#include "parser.h"


command_t getCommand(char* str){
	char* resStr;
	command_t command;
	resStr = strstr(str, TAG_COMMAND);
	if(resStr == NULL){
		return (command_t){NULL, NULL};
	}
	resStr += strlen(TAG_COMMAND);
	command.name = resStr; // Le nom de la commande est ici, il faut juste couper la chaine au bon endroit
	str = substrpbrk(resStr, " [\n");
	return command;
}
