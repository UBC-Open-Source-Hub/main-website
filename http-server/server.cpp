// Standard libraries
#include <cstring>
#include <string>
#include <cstdio>

// Network Related libraries
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "inc/server.h"

const char *TARGET_PORT = "8080";

int main (int argc, char *argv[]) {
   printf("Starting server....\n");
   struct addrinfo hints, *res ;
   int rc = 0; // Return code
   int fd = 0; // Socket file descriptor

   // Initialize the hints
   memset(&hints, 0, sizeof(hints));
   hints.ai_family   = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags    = AI_PASSIVE | AI_ADDRCONFIG;

   rc = getaddrinfo(nullptr, TARGET_PORT, &hints, &res);
   if (rc) {
      printf("Failed to start the server with %d\n", rc);
      goto EXIT;
   }

   // Go through the linked list and bind to the first "good" result
   for (struct addrinfo *traveler = res; traveler != nullptr; traveler = traveler->ai_next) {
      std::string ipStr  = getIpAddrStr(traveler->ai_addr);
      const char *ipCStr = ipStr.c_str();
      printf("Found %s\n", ipCStr);

      fd = socket(traveler->ai_family, traveler->ai_socktype, traveler->ai_protocol);
      if (-1 == fd) {
         printf("Failed to create a socket with %s\n", ipCStr);
         continue;
      }

      rc = bind(fd, traveler->ai_addr, traveler->ai_addrlen);
      if (rc) {
         printf("Failed to bind address %s to port %s", ipCStr, TARGET_PORT);
         continue;
      }

      break;
   }

   if (-1 == fd) {
      rc = -1;
      printf("Failed to find a good address to bind to\n");
      goto EXIT;
   }

EXIT:
   if (nullptr != res) {
      freeaddrinfo(res);
   }
   return rc;
}


// Get string presentation of a socket address
std::string getIpAddrStr(struct sockaddr *addr) {
   char ip[INET6_ADDRSTRLEN]  = {0};
   void *addrIn = nullptr ;

   if (AF_INET == addr->sa_family) { // IPV4
      struct sockaddr_in *addrV4 = (sockaddr_in *)addr;
      addrIn = &(addrV4->sin_addr);
   } else if (AF_INET6 == addr->sa_family) { // IPV6
      struct sockaddr_in6 *addrV6 = (sockaddr_in6 *)addr;
      addrIn = &(addrV6->sin6_addr);
   } else {
      return "";
   }

   inet_ntop(addr->sa_family, addrIn, ip, sizeof(ip));

   return ip;
}
