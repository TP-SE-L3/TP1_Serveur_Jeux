#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

#include "coreS.h"

int receptData(int fd, SOCKET s) {
  char buff[SIZE_BUFF];
  int res, res2;
  int nb_recu = 0;
  while(1) {
    res = recv(s, buff, SIZE_BUFF, 0);
    if (res < 0) {
      perror("Probleme a la lecture du fichier");
      return -1;
    }
    if (res == 0) {
      // Le fichier est terminé
      break;
    }
    nb_recu += res;
    //    fprintf(stderr, "Recu %d oct total %d oct\n", res, nb_recu);
    res2 = write(fd, buff, res);
    if (res != res2) {
      perror("Probleme a l'ecriture du fichier");
      return -1;
    }
  }
  return nb_recu;
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
