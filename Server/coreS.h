#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <error.h>
#include <string.h>
#include <stdio.h>

/* Une partie nécessaire pour utiliser les sockets sous linux et windows */
#if defined (WIN32)
#include <winsock2.h>
#elif defined (linux) // Sous linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#else
#error Le programme ne fonctionne pas pour cette platforme
#endif

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1


#ifndef NI_MAXHOST
#define NI_MAXHOST
#endif


extern SOCKET sock_to_close;
extern char * program_name;
void terminaison(int signal);

SOCKET CreeSocketServeur(const char* port);
int sendMessage(SOCKET s, char* format, ...);

int EnvoieFichier(SOCKET s, int fd);
int RecoitFichier(int fd, SOCKET s);
