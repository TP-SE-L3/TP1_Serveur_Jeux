#include <sys/types.h>


#include "coreS.h"


SOCKET CreeSocketServeur(const char* port) {
  
  SOCKET s=-1;
  // structure pour faire la demande de port
  struct addrinfo hints;
  // structure pour stocker et lire les résultats
  struct addrinfo *result, *rp;
  // variables pour tester si les fonctions donnent un résultats ou une erreur 
  int bon, res;
  // Des variable pour contenir de adresse de machine et des numero de port afin de les afficher
  char hname[NI_MAXHOST], sname[NI_MAXSERV];

  // ######################################
  // mise en place de la connexion
  // ######################################
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* IPv4 ou IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* socket flux connectée */
  hints.ai_flags = AI_PASSIVE;    /* Signifie que toutes les addresse de la machine seront utilisée */
  hints.ai_protocol = 0;          /* Any protocol */
  // on effectue la demande pour le port demandé
  
  // ------------------------ TODO --------------------------
  
  // ------------------------
  
  res = getaddrinfo(NULL, port, &hints, &result);
  if (res != 0) { // c'est une erreur
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
    exit(1);
  }
  else{

    s = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
    // si le résultat est -1 cela n'a pas fonctionné on recommence avec la prochaine
    if (s == -1) {
      perror("Création de la socket");
      rp = rp->ai_next;
      continue;
    }    
    
    // partie optionnelle pour éviter d'être rejeté par le système si le précédant test a planté
    res = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes,
		     sizeof(int));
    if (res == -1) {
            perror("setsockopt");
	    rp = rp->ai_next;
            continue;
    }
    // fin de la partie optionnelle

    // si la socket a été obtenue, on essaye de réserver le port
    res = bind(s, rp->ai_addr, rp->ai_addrlen);
    if (res == 0 ) {// cela a fonctionné on affiche l'information
      bon = 1;

      // on récupère des informations affichables
      res = getnameinfo(rp->ai_addr, rp->ai_addrlen,
			hname, NI_MAXHOST,
			sname, NI_MAXSERV,
			NI_NUMERICSERV|NI_NUMERICHOST);
      if (res != 0) {
	fprintf(stderr, "getnameinfo: %s\n", gai_strerror(res));
	exit (1);
      }
      printf ("La socket %d est maintenant en attente sur l'adresse %s le port %s\n",
	      s, hname, sname); 
      break; 
    }
    else { // sinon le bind a été impossible, il faut fermer la socket
      perror("Imposible de réserver l'adresse");
      close (s);
    }

    rp = rp->ai_next;
  }

  if (bon == 0) { // Cela n'a jamais fonctionné
    fprintf(stderr, "Impossible de faire un bind\n");
    exit(1);
  }

  
  // on libère la structure devenue inutile
  freeaddrinfo(result);
  
  sock_a_fermer = s;
  res = listen (s, 5);
  if (res < 0) {
    perror("listen");
    terminaison(0);
  }
  

  return s;
}
