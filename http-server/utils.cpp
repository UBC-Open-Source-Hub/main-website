#include <arpa/inet.h>
#include <signal.h>

#include "inc/utils.h"

// Forward declaration (to hide internal helper functions)
void setSignal(int action, int sig) ;

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

// Utility function to block certain signal around critical code
void blockSignal(int sig) {
   setSignal(SIG_BLOCK, sig);
}

// Utitlity function to unblock certain signal 
void unBlockSignal(int sig) {
   setSignal(SIG_UNBLOCK, sig);
}

// Internal helper function blockSignal and unBlockSignal
void setSignal(int action, int sig) {
   sigset_t newSet;
   sigemptyset(&newSet);
   sigaddset(&newSet, sig);
   sigprocmask(action, &newSet, nullptr);
}
