#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include "core.h"
#include "utils.h"
#include "parser.h"
#include "linkedlist.h"



SOCKET connectCli(const char* servIP){
	struct sockaddr_in sin = {0};
	int sockOptions = 1;
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
			perror("Error socket");
			exit(EXIT_FAILURE);
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(servIP);
	
	// Permet de ne pas avoir d'erreur de type : endpoint is not connected
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockOptions, sizeof(sockOptions)) == -1){
		perror("Erreur avec la fonction setsockopt()");
		exit(EXIT_FAILURE);
	}
	
	if(connect(sock, (struct sockaddr*)&sin, sizeof(sin)) == -1){
		perror("Erreur pendant la connexion au serveur.");
		exit(EXIT_FAILURE);
	}
	return sock;
}


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

int sendHeader(SOCKET sock, header_t header){
	char message[SIZE_HEADER+1];
	int i;
	sprintf(message, "[ID:%d ID_GAME:%d SIZE:%d]", header.id, header.idGame, header.size);
	for(i=strlen(message); i < SIZE_HEADER; i++){
		message[i] = ' '; // On remplit la fin de chaine avec des espaces
	}
	message[SIZE_HEADER] = '\0';
	printf("Header send : %s\n", message);
	fflush(stdout);
	return sendMessage(sock, "%s", message); // Ici la va_list ne sert à rien
}


int recvHeader(SOCKET sock, header_t* header){
	char message[SIZE_HEADER];
	char* strCurrent; // Chaine qui va être modifée, petit à petit pour récupérer les bon éléments
	char* startStrCurrent; // Le pointeur de départ de strCurrent, pour libérer la mémoire à la fin
	char* idStr, *sizeStr;
	char* resStr; // Tmp variable de résultat
	/**
	 * Améliorer la fonction de reception ----------------------- TODO
	 * */
	if(recv(sock, message, sizeof(message), 0) == -1){
		perror("Error recv");
		return -1;
	}
	// Revoir le nom des variables
	// Les paramètres doivent être dans le bon ordre (1:ID, 2:SIZE)
	strCurrent = strbtw(message, '[', ']');
	startStrCurrent = strCurrent;
	resStr = strstr(strCurrent, "ID:"); // Récupère l'id du joueur
	if(resStr != NULL){
		strCurrent = resStr;
		idStr = strCurrent+strlen("ID:");
		strCurrent = substrpbrk(idStr, " \0");
		header->id = atoi(idStr); // Peut utiliser strtol, avec gestion d'erreur
	}
	resStr = strstr(strCurrent, "ID_GAME:"); // Récupère l'id du jeu
	if(resStr != NULL){
		strCurrent = resStr;
		idStr = strCurrent+strlen("ID_GAME:");
		strCurrent = substrpbrk(idStr, " \0");
		header->idGame = atoi(idStr); // Peut utiliser strtol, avec gestion d'erreur
	}
	resStr = strstr(strCurrent, "SIZE:"); // La taille du message suivant
	if(strCurrent != NULL){
		strCurrent = resStr;
		sizeStr = strCurrent+strlen("SIZE:");
		strCurrent = substrpbrk(sizeStr, " \0");
		header->size = atoi(sizeStr);
	}
	free(startStrCurrent);
	return 0;
}


char* recvMessage(SOCKET sock, header_t header){
	char* message = malloc(header.size+1 * sizeof(char));
	
	if(recv(sock, message, header.size, 0) == -1){
		perror("Error recv");
		return NULL;
	}
	*(message + header.size) = '\0';
	return message;
}
