#include <sys/types.h>
#include <netdb.h>

#include "coreC.h"

SOCKET sock_to_close = -1;
char * program_name = "Test";

SOCKET CreeConnectSocketClient(const char *nom_serveur, const char* port) {
  // structure pour faire la demande
  struct addrinfo hints;
  // structure pour stocker et lire les résultats
  struct addrinfo *result, *rp;

   // variables pour tester si les fonctions donnent un résultats ou une erreur
  int res;
  int bon;
  // Des variable pour contenir de adresse de machine et des numero de port afin de les afficher
  char hname[NI_MAXHOST], sname[NI_MAXSERV];
  // la socket utilisée
  SOCKET s=-1;

  // on rempli la structure hints de demande d'adresse
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* IPv4 ou IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* socket flux connectée */

  res = getaddrinfo(nom_serveur, port, &hints, &result);
  if (res != 0) { // c'est une erreur
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
    exit(1);
  }
  
  // si res = 0 le véritable résultat de la fontion est l'argument result
  // qui contient une liste d'addresse correspondant à la demande on va les
  // rester jusqu'a trouver une qui convient
  rp = result;
  bon = 0;
 
  while (rp != NULL) {
    // on parcourt la liste pour en trouver une qui convienne

    // on récupère des informations affichables
    res = getnameinfo(rp->ai_addr, rp->ai_addrlen,
		      hname, NI_MAXHOST,
		      sname, NI_MAXSERV,
		      NI_NUMERICSERV|NI_NUMERICHOST);
    if (res != 0) {
      fprintf(stderr, "getnameinfo: %s\n", gai_strerror(res));
      exit (1);
    }
    fprintf (stderr, "On tente l'adresse %s sur le port %s .....",
	    hname, sname);
    
    // on essaye
    s = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
    // si le résultat est -1 cela n'a pas fonctionné on recommence avec la prochaine
    if (s == -1) {
      perror("Création de la socket");
      rp = rp->ai_next;
      continue;
    }    
   
    // si la socket a été obtenue, on essaye de se connecter
    res = connect(s, rp->ai_addr, rp->ai_addrlen);
    if (res == 0 ) {// cela a fonctionné on est connecté
      bon = 1;
      fprintf (stderr, "OK\n");
      break; 
    }
    else { // sinon le bind a été impossible, il faut fermer la socket
      perror("Imposible de se connecter");
      close (s);
    }

    rp = rp->ai_next;
  }

  if (bon == 0) { // Cela n'a jamais fonctionné
    fprintf(stderr, "Aucune connexion possible\n");
    exit(1);
  }
  freeaddrinfo(result);           /* No longer needed */  

  sock_a_fermer = s;

  return s;
}


void terminaison(int signal) {
  if (signal != 0) {
    fprintf(stderr, "Le programme %s se termine car il a recut le signal %d\n", program_name, signal);
  } else {
    fprintf(stderr, "Fin du programme %s\n", program_name);
  }
  if (sock_to_close != INVALID_SOCKET) {
    fprintf(stderr, "Fermeture de la socket\n");
    close(sock_to_close);
  }

#if defined (WIN32)
  // Ceci est du code spécifique à windows
  WSACleanup();
#endif
}
  
