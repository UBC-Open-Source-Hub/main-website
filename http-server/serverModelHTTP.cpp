#include "inc/serverModelHTTP.h"
#include "inc/serverModel.h"
#include "inc/utils.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

ServerModelHTTP::ServerModelHTTP() : ServerModel() {
   for (int i = 0; i < NUM_WORKERS; i++) {
      std::thread *newWorker = new std::thread(&ServerModelHTTP::startWorker, this);
      this->workers.push_back(newWorker);
   }
}

ServerModelHTTP::~ServerModelHTTP() {
   for (auto &pWorker: this->workers) {
      delete pWorker;
   }
}

int ServerModelHTTP::acceptConnections(int socket) {
   // send a socket fd to each the job queue
   // use a condition_variable to wake up one of the worker
   // let they work and process the connections
   // and the end of each connection, go back to sleep (aka waiting for the condition variable)
   this->socket = socket;
   int maxFd = std::max(this->pipeFds[0], this->socket) ;

   while (this->isActive) {
      struct sockaddr_storage clientAddr;
      socklen_t addrSize = sizeof(clientAddr);

      fd_set targetFds;
      FD_ZERO(&targetFds);
      FD_SET(this->socket, &targetFds);
      FD_SET(this->pipeFds[0], &targetFds);
      int selRes = select(maxFd + 1, &targetFds, nullptr, nullptr, nullptr);
      if (-1 == selRes && errno != EINTR) {
         eprintf("Failed to wait for either pipe or socket to be ready");
         continue;
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
         }
         continue;
      }

      processConnection(fd);
   }

   return 0;
}

void ServerModelHTTP::processConnection(int fd) {
   this->jobs.push(fd);
   this->condVar.notify_all();
   
}

void ServerModelHTTP::deactivate() volatile {

}

void ServerModelHTTP::shutdownConnections() {
   exit(0);
}

void ServerModelHTTP::startWorker() {
   while (this->isActive) {
      int fd = -1;
      char buffer[100] = {0};
      int receivedLen = 0;
      {
         std::unique_lock<std::mutex> lock(this->mu);
         this->condVar.wait(lock, [this]{ return !this->isActive || this->jobs.size() > 0; });
         if (!this->isActive)
            break;
         fd = this->jobs.front();
         this->jobs.pop();
      }

      receivedLen = recv(fd, buffer, sizeof(buffer), 0);
      if (0 == receivedLen) {
         printf("Remote connection has been closed\n");
      }
      std::cout << "Thread " << std::this_thread::get_id() << " received: " << buffer << "\n";
   }
}
