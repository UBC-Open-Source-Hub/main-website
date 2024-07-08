#include "inc/serverModelHTTP.h"
#include <cstdio>

int ServerModelHTTP::acceptConnections(int socket) {
   printf("Accepting new HTTP connections\n");
   return 0;
}

void ServerModelHTTP::processConnection(int fd) {

}

void ServerModelHTTP::deactivate() volatile {

}

void ServerModelHTTP::shutdownConnections() {

}
