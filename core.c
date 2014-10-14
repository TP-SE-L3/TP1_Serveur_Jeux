#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include "core.h"
#include "utils.h"


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
	char* contain;
	int res;
	/**
	 * Améliorer la fonction de reception
	 * */
	if(recv(sock, message, sizeof(message), 0) == -1){
		perror("Error recv");
		return -1;
	}
	printf("%s --- s:%d\n", message, strlen(message));
	res = stroc("Bonjour", "");
	contain = strbtw(message, '[', ']');
	printf("Contain : %s\n", contain);
	free(contain);
	printf("Res : %d", res);
	return 0;
}

int sendHeader(SOCKET sock, header_t header){
	char message[SIZE_HEADER] = "[ID:%d SIZE:%d]";
	return sendMessage(sock, message, header.id, header.size);
}
