#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

#include "inc/log.h"
#include "inc/securityModel.h"
#include "inc/securityModelSimple.h"

SecurityModelSimple::SecurityModelSimple(int socket) : SecurityModel(socket) {}
SecurityModelSimple::~SecurityModelSimple() {}

int SecurityModelSimple::acceptConnections() {
   printf("Begin accepting new connections\n");
   this->isActive = true;

   while(this->isActive) {
      struct sockaddr_storage clientAddr;
      socklen_t addrSize = sizeof(clientAddr);

      // Thread will block if there are no incoming connections
      int fd = accept(this->socket, (struct sockaddr *)&clientAddr, &addrSize);
      if (-1 == fd) {
         eprintf("Failed to accept new connections");
         return -1;
      }

      while (true) { // TODO: Spawn a new thread to deal with the connections
         char buffer[100] = {0};
         int receivedLen = recv(fd, buffer, sizeof(buffer), 0);
         if (0 == receivedLen) {
            printf("Remote connection was closed by client\n");
            close(fd);
            break;
         }
         printf("Received message: %s", buffer);
      }
      break; // TODO: Remove this
   }

   return 0;
}
