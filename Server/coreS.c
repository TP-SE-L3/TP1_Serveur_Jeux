#include <sys/types.h>
#include <netdb.h>
#include <stdarg.h>
#include <errno.h>

#include "coreS.h"

SOCKET sock_to_close = -1;
char * program_name = "Test";

SOCKET CreeSocketServeur(const char* port) {
	
	SOCKET sock;
	struct addrinfo hints; // structure pour faire la demande de port
	struct addrinfo *listAddr, *address = NULL; // structure pour stocker et lire les résultats
	// variables pour tester si les fonctions donnent un résultats ou une erreur 
	int res, valOpt = 1;
	// Des variable pour contenir de adresse de machine et des numero de port afin de les afficher
	char hname[NI_MAXHOST], sname[NI_MAXSERV];

	// ######################################
	// mise en place de la connexion
	// ######################################
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* IPv4 ou IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* socket flux connectée */
	hints.ai_flags = AI_PASSIVE;    // Toutes les addresses de la machine seront utilisées
	hints.ai_protocol = 0;          //Any protocol
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	/* getAddrInfo : Crée une ou plusieurs structures d'adresses de socket
	 hints est une sorte de filtre permettant de séléctionner les adresses que l'on veut */
	res = getaddrinfo(NULL, port, &hints, &listAddr); 
	if (res != 0) { // c'est une erreur
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
		exit(EXIT_FAILURE);
	}
	for(address = listAddr; address != NULL; address = address->ai_next){
		sock = socket(address->ai_family, address->ai_socktype, address->ai_protocol);

	if (sock == INVALID_SOCKET) {
		perror("Création de la socket");
		continue;
	}    

	/* -----------------------------------------------------
	partie optionnelle pour éviter d'être rejeté par le système si le précédant test a planté*/
	res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &valOpt, sizeof(int));
	if (res == -1) {
		perror("setsockopt");
		continue;
	}
	/* fin de la partie optionnelle
	-----------------------------------------------------*/

	// On essaye de réserver le port
	res = bind(sock, address->ai_addr, address->ai_addrlen);
	if (res == 0 ) {// cela a fonctionné on affiche l'information
		// on récupère des informations affichables
		res = getnameinfo(address->ai_addr, address->ai_addrlen,
			hname, NI_MAXHOST,
			sname, NI_MAXSERV,
			NI_NUMERICSERV|NI_NUMERICHOST);
		if (res != 0) {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(res));
			exit (EXIT_FAILURE);
		}
		printf ("La socket %d est maintenant en attente sur l'adresse %s le port %s\n", sock, hname, sname); 
		break; // On quite la boucle
		}
	else { // bind impossible
		perror("Imposible de reserver l'adresse");
		close(sock);
	}
  }

  if (address == NULL) { // Aucun socket n'a pu être réservé sur un port
    fprintf(stderr, "Impossible de faire un bind\n");
    exit(EXIT_FAILURE);
  }

  // on libère la structure devenue inutile
  freeaddrinfo(listAddr);
  
  res = listen (sock, 5);
  if (res < 0) {
    perror("listen");
    terminaison(0);
  }
  
  return sock;
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



int EnvoieMessage(SOCKET s, char* format, ...){
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


/* -------------------------------------------------
				Avec formatage des données
 -------------------------------------------------*/
/*int sendMessage(SOCKET s, char* format, ...){
	int i;
	int res;

	va_list listArgs;
	va_start(listArgs, format); // format : le dernier paramètre nommé de la fonction
	
	// -------------- RAJOUTER LES TABLEAUX (pointeurs) -------------
	for(i=0; format[i] != '\0'; i++){
		switch(format[i]){
			case 'c': // Caractère
				printf("C : %c\n", va_arg(listArgs, int));
			break;
			case 'i': // Int
				printf("I : %d\n", va_arg(listArgs, int));
			break;
			case 'f': // Float
				printf("F : %f\n", va_arg(listArgs, double));
			break;
			case 'd': // Double
				printf("D : %lf\n", va_arg(listArgs, double));
			break;
			case 's': // String
				printf("S : %s\n", va_arg(listArgs, char*));
			break;
			default: break;
		}
	}
	va_end(listArgs);

	/*i = 0;
	while (i < str->len) {
		res = send(s, str->str+i, str->len-i, MSG_NOSIGNAL);
		printf("str : %s\n", str->str);
		if(res<=0){
		  fprintf(stderr, "error: write %s car %s\n", str->str, strerror(errno));
		  return -1;
		}
		i += res;
	}

	g_string_free(str, TRUE);
	return 0;
}
*/



/* -------------------------------------------------------
				Avec glib (ne fonctionne pas)
 ---------------------------------------------------------*/
/*
int sendMessage(SOCKET s, char* format, ...){
  int i;
  int res;

  va_list listArgs;
  va_start(listArgs, format); // format : le dernier paramètre nommé de la fonction
  GString *str = g_string_new("");
  g_string_vprintf(str, format, listArgs);

  va_end(listArgs);

  i = 0;
  while (i < str->len) {
    res = send(s, str->str+i, str->len-i, MSG_NOSIGNAL);
    printf("str : %s\n", str->str);
    if(res<=0){
      fprintf(stderr, "error: write %s car %s\n", str->str, strerror(errno));
      return -1;
    }
    i += res;
  }

  g_string_free(str, TRUE);
  return 0;
}*/
  
