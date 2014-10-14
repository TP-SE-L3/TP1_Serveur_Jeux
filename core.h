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
#define PORT 2000
#define SIZE_HEADER 20

#define TRUE 1
#define FALSE 0

#ifndef NI_MAXHOST
#define NI_MAXHOST
#endif



typedef struct header_t header_t;
struct header_t{
	int id;
	int size;
};


SOCKET CreeConnectSocketClient(const char *nom_serveur, const char* port);

int sendMessage(SOCKET s, char* format, ...);

int recvHeader(SOCKET sock, struct header_t* header);
int sendHeader(SOCKET sock, header_t header);
