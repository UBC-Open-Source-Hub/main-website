#include "inc/utils.h"

#include <arpa/inet.h>

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
