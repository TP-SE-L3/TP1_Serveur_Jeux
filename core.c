#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include "core.h"
#include "utils.h"
#include "parser.h"


int sendMessage(SOCKET s, char* format, ...){
  int i;
  int res;

  va_list listArgs;
  va_start(listArgs, format);

  // on calcul la taille du message
  int taille = vsnprintf(NULL, 0, format, listArgs);
  va_end(listArgs);

  // un tableau un peu plus grand pour le \0
  char chaine[taille+1];

  va_start(listArgs, format);
  vsnprintf(chaine, taille+1, format, listArgs);
  va_end(listArgs);

  i = 0;
  while (i < taille) { // attention, il ne faut pas envoyer le \0
    res = send(s, chaine+i, taille-i, MSG_NOSIGNAL);
    if(res<=0){
      fprintf(stderr, "error: write %s car %s\n", chaine, strerror(errno));
      return -1;
    }
    i += res;
  }

  return i;
}

int recvHeader(SOCKET sock, header_t* header){
	char message[SIZE_HEADER];
	char* strCurrent; // Chaine qui va être modifée, petit à petit pour récupérer les bon éléments
	char* idStr, *sizeStr;
	char* resStr; // Tmp variable de résultat
	/**
	 * Améliorer la fonction de reception ----------------------- TODO
	 * */
	if(recv(sock, message, sizeof(message), 0) == -1){
		perror("Error recv");
		return -1;
	}
	printf("%s-->s:%d\n", message, strlen(message));
	// Revoir le nom des variables
	// Les paramètres doivent être dans le bon ordre (1:ID, 2:SIZE)
	strCurrent = strbtw(message, '[', ']');
	resStr = strstr(strCurrent, "ID:");
	if(resStr != NULL){
		strCurrent = resStr;
		idStr = strCurrent+strlen("ID:");
		strCurrent = substrpbrk(idStr, " \0");
		header->id = atoi(idStr); // Peut utiliser strtol, avec gestion d'erreur
	}
	resStr = strstr(strCurrent, "SIZE:");
	if(strCurrent != NULL){
		strCurrent = resStr;
		sizeStr = strCurrent+strlen("SIZE:");
		strCurrent = substrpbrk(sizeStr, " \0");
		header->size = atoi(sizeStr);
	}
	free(strCurrent);
	return 0;
}

int recvMessage(SOCKET sock, header_t header){
	char* message = malloc(header.size * sizeof(char));
	command_t commande;
	if(recv(sock, message, header.size, 0) == -1){
		perror("Error recv");
		return -1;
	}
	printf("Message %s\n", message);
	commande = getCommand(message);
	printf("Commande: %s\n", commande.name);
	//printf("Commande: %s\n", commande.strArgs);
	
	free(message);
	return 0;
}


int sendHeader(SOCKET sock, header_t header){
	char message[SIZE_HEADER];
	int i;
	sprintf(message, "[ID:%d SIZE:%d]", header.id, header.size);
	for(i=strlen(message); i < SIZE_HEADER; i++){
		message[i] = ' '; // On remplit la fin de chaine avec des espaces
	}
	return sendMessage(sock, "%s", message); // Ici la va_list ne sert à rien
}
