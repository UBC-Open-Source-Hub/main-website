#include <cstdio>
#include <iostream>
#include <mutex>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <utility>

#include "inc/utils.h"
#include "inc/securityModel.h"
#include "inc/securityModelSimple.h"

SecurityModelSimple::SecurityModelSimple(int socket) : SecurityModel(socket) {}
SecurityModelSimple::~SecurityModelSimple() {}

int SecurityModelSimple::acceptConnections() {
   printf("Begin accepting new connections\n");
   this->isActive = true;

   while(this->isActive) {
      // printf("Current number of connections: %zu", this->workers.size());
      struct sockaddr_storage clientAddr;
      socklen_t addrSize = sizeof(clientAddr);

      // Thread will block if there are no incoming connections
      int fd = accept(this->socket, (struct sockaddr *)&clientAddr, &addrSize);
      if (-1 == fd) {
         eprintf("Failed to accept new connections");
         return -1;
      }

      std::unique_lock<std::mutex> lck(this->mutex);
      std::thread newWorker(&SecurityModelSimple::processConnection, this, fd);
      newWorker.detach();
      this->workers.insert(std::make_pair<std::thread::id, std::thread>(newWorker.get_id(), std::move(newWorker)));
      lck.unlock();
   }

   return 0;
}

void SecurityModelSimple::processConnection(int fd) {
   std::thread::id id = std::this_thread::get_id();
   while (true) { // TODO: Spawn a new thread to deal with the connections
      char buffer[100] = {0};
      int receivedLen = recv(fd, buffer, sizeof(buffer), 0);
      if (0 == receivedLen) {
         printf("Remote connection was closed by client\n");
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
