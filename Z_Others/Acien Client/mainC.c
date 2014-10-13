#include <stdlib.h>
#include <stdio.h>

#include "coreC.h"

/* Main coté client */


int main(){
		const char* server_name = "0.0.0.0";
		const char* port = "1025";
		int res;
		SOCKET sock;
		
		sock = CreeConnectSocketClient(server_name, port);
		//printf("Socket : %d\n", res);
		
		while(receptData(stdout, sock) <= 0){
			//printf("-");
		}
			
		return 0;
}
