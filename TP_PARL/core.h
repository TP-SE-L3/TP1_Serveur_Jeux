#ifndef CORE_H
#define CORE_H

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
#include <unistd.h>
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
#define SIZE_HEADER 40

#define TRUE 1
#define FALSE 0

#ifndef NI_MAXHOST
#define NI_MAXHOST
#endif


/* Le header est envoyé à chaque fois au client, au cas ou il y a un changement 
(Ex: le serveur décide de change l'id du joueur)*/
typedef struct header_t header_t;
struct header_t{
	int id;
	int idGame;
	int size;
};

/*Peremet d'initialiser un socket Serveur
 * Retourne la socket qui à été initialisée
 * @param servIP: Adresse ip du serveur sous le format : "w.x.y.z" 
 * @param nbCli: nombre de client en même temps sur le endpoint (Inutile pour un serveru séquentiel)*/
SOCKET connectServ(const char* servIP, int nbCli, SOCKADDR_IN* addrServ);

/*Peremet d'initialiser une connexion client
 * Retourne la socket qui à été initialisée
 * @param servIP: Adresse ip du serveur sous le format : "w.x.y.z" */
SOCKET connectCli(const char* servIP);

SOCKET CreeConnectSocketClient(const char *nom_serveur, const char* port);

int sendMessage(SOCKET s, char* message);

int sendHeader(SOCKET sock, header_t header);

int recvHeader(SOCKET sock, header_t* header);

/* Retourne une chaine contenant le message reçus
 * NULL si erreur, Attention la place devra être libérée
 * @param sock: socket sur lequel il faut lire
 * @param header: structure contenant les informations sur le message
*/
char* recvMessage(SOCKET sock, header_t header);

#endif
