// Standard libraries
#include <csignal>
#include <cstring>
#include <mutex>
#include <string>
#include <cstdio>

// Network Related libraries
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "inc/utils.h"
#include "inc/serverModel.h"
#include "inc/serverModelSimple.h"

// The port the server will listen on
const char *TARGET_PORT = "8080";
// How many pending connections can be in a queue
const int  BACKLOG = 10;

static volatile sig_atomic_t isExiting = false;
std::mutex sigHandlerMutex;
volatile ServerModel *securityModel = nullptr;

void signalHandler(int sigNum) {
   // Ensure lock is released at the end of scope
   {
      std::unique_lock<std::mutex> lck(sigHandlerMutex);
      // Avoid re-entrancy!!!
      if (isExiting)
         return;
      isExiting = true;
   }


   printf("Stopping the server...\n");
   if (securityModel == nullptr)
      return;

   securityModel->deactivate();
}

int main (int argc, char *argv[]) {
   // Register signal handler for interrupt signal
   signal(SIGINT, signalHandler);

   printf("Starting server....\n");
   struct addrinfo hints, *res ;
   int rc = 0; // Return code
   int socketFd = 0; // Socket file descriptor
   
   // Initialize the hints
   memset(&hints, 0, sizeof(hints));
   hints.ai_family   = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags    = AI_PASSIVE | AI_ADDRCONFIG;

   rc = getaddrinfo(nullptr, TARGET_PORT, &hints, &res);
   if (rc) {
      eprintf("Failed to obtain the server's address info");
      return rc;
   }

   // Go through the linked list and bind to the first "good" result
   for (struct addrinfo *traveler = res; traveler != nullptr; traveler = traveler->ai_next) {
      std::string ipStr  = getIpAddrStr(traveler->ai_addr);
      const char *ipCStr = ipStr.c_str();
      printf("Found %s\n", ipCStr);

      socketFd = socket(traveler->ai_family, traveler->ai_socktype, traveler->ai_protocol);
      if (-1 == socketFd) {
         eprintf("Failed to create a socket with %s", ipCStr);
         rc = -1;
         continue;
      }

      rc = bind(socketFd, traveler->ai_addr, traveler->ai_addrlen);
      if (rc) {
         eprintf("Failed to bind address %s to port %s", ipCStr, TARGET_PORT);
         continue;
      }

      break;
   }
   freeaddrinfo(res);
   if (rc) {
      eprintf("Failed to find a good address to bind to");
      return rc;
   }

   // Start listening
   rc = listen(socketFd, BACKLOG);
   if (-1 == socketFd) {
      eprintf("Failed to start listening on port %s", TARGET_PORT);
      return -1;
   } 

   ServerModelSimple simpleModel;
   securityModel = &simpleModel;
   simpleModel.acceptConnections(socketFd);

   return rc;
}
