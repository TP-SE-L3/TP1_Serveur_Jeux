#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <error.h>
#include <string.h>
#include <stdio.h>

/* Une partie nécessaire pour utiliser les sockets sous linux et windows */
#if defined (WIN32)
#include <winsock2.h>
#elif defined (linux)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define TRUE 1
#define FALSE 0



typedef int SOCKET;

extern SOCKET sock_a_fermer;
extern char * nom_programme;
void terminaison(int signal);

SOCKET CreeSocketServeur(const char* port);
SOCKET CreeConnectSocketClient(const char *nom_serveur, const char* port);

int EnvoieFichier(SOCKET s, int fd);
int RecoitFichier(int fd, SOCKET s);
