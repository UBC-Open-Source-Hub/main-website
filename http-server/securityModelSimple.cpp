#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <sys/select.h>
#include <fcntl.h>
#include <vector>

#include "inc/utils.h"
#include "inc/securityModelSimple.h"

int SecurityModelSimple::acceptConnections(int socket) {
   printf("Begin accepting new connections\n");

   // Initialize communication
   if (-1 == pipe(this->pipeFds)) {
      eprintf("Failed to create self-pipe");
      return -1;
   }
   // Change model state
   this->isActive = true;
   // register the listening socket
   this->socket = socket;
   // setting up the file descriptors that we'll be working with
   int maxFd = std::max(this->pipeFds[0], this->socket);

   while(true) {
      struct sockaddr_storage clientAddr;
      socklen_t addrSize = sizeof(clientAddr);

      fd_set targetFds;
      FD_ZERO(&targetFds);
      FD_SET(this->socket, &targetFds);
      FD_SET(this->pipeFds[0], &targetFds);
      int selRes = select(maxFd + 1, &targetFds, nullptr, nullptr, nullptr);
      if (-1 == selRes && errno != EINTR) {
         eprintf("Failed to wait for either pipe or socket to be ready");
         break;
      }

      if (FD_ISSET(this->pipeFds[0], &targetFds)) {
         printf("Stop accepting new connections\n");
         break;
      }
      
      // Thread will block if there are no incoming connections
      int fd = accept(this->socket, (struct sockaddr *)&clientAddr, &addrSize);
      if (-1 == fd) {
         if (errno != EWOULDBLOCK) {
            eprintf("Failed to accept new connections");
            return -1;
         }
         continue;
      }

      std::unique_lock<std::mutex> lck(this->mutex);
      std::thread newWorker(&SecurityModelSimple::processConnection, this, fd);
      std::thread::id id = newWorker.get_id(); 
      newWorker.detach();
      this->workers[id] = std::make_pair(std::move(newWorker), fd);
      lck.unlock();
   }

   close(this->socket);
   close(this->pipeFds[0]);
   shutdownConnections();

   return 0;
}

void SecurityModelSimple::processConnection(int fd) {
   std::thread::id id = std::this_thread::get_id();
   while (true) {
      char buffer[100] = {0};
      int receivedLen = recv(fd, buffer, sizeof(buffer), 0);
      if (0 == receivedLen) {
         printf("Remote connection has been closed\n");
         close(fd);
         break;
      }
      std::cout << "Thread " << id << " received: " << buffer << "\n";
   }
   
   // Remove from list of workers after finishing
   std::unique_lock<std::mutex> lck(this->mutex);
   this->workers.erase(id);
   lck.unlock();
}

void SecurityModelSimple::deactivate() volatile {
   printf("Deactivating the model\n");
   if (!this->isActive)
      return;

   if (-1 == close(this->pipeFds[1])) {
      eprintf("Unable to deactivate the security model");;
      return;
   }
}

void SecurityModelSimple::shutdownConnections() {
   std::vector<std::thread::id> keys;
   for (auto &entry: this->workers) {
      auto &[ id, workerAndFd ] = entry;
      keys.push_back(id);
   }

   const char *closeMsg = "Closing the connection";
   for (auto &k: keys) {
      auto &[ worker, fd ] = this->workers.at(k);
      send(fd, closeMsg, strlen(closeMsg), 0);
      shutdown(fd, 0); // Stop recving
   }

   // Make sure all the workers have shutdown
   while (this->workers.size() != 0) {
      sleep(1);
   }
}
