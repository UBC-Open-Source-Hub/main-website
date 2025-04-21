#include "inc/serverModelHTTP.h"
#include "inc/responseHTTP.h"
#include "inc/serverModel.h"
#include "inc/requestHTTP.h"
#include "inc/utils.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <semaphore>
#include <shared_mutex>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <string>

ServerModelHTTP::ServerModelHTTP() : ServerModel(), sem(0) {
   for (int i = 0; i < 1; i++) {
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
   // Initialize communication
   if (-1 == pipe(this->pipeFds)) {
      eprintf("Failed to create self-pipe");
      return -1;
   }
   this->socket = socket;
   this->isActive = true;

   fd_set targetFds;
   int maxFd = std::max(this->pipeFds[0], this->socket) ;

   while (this->isActive) {
      struct sockaddr_storage clientAddr;
      socklen_t addrSize = sizeof(clientAddr);

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
   this->jobQueue.pushJob(fd);
   this->sem.release();
}

void ServerModelHTTP::deactivate() volatile {

}

void ServerModelHTTP::shutdownConnections() {
   exit(0);
}

void ServerModelHTTP::startWorker() {
   std::cout << "Thread " << std::this_thread::get_id() << " entering....\n\n";

   char buffer[HEADER_MAX_SIZE + 1] = {0};
   std::string bufferStr            = "";


   RequestHTTP  req;
   ResponseHTTP resp;
   RequestHeaderHTTP *reqHeader = nullptr;
   
   std::string errMsg = "";
   StatusCode statusCode = StatusCode::OK;

   while (this->isActive) {
      int receivedLen = 0;

      this->sem.acquire(); 
      if (this->jobQueue.size() == 0) // could be woken up to stop the thread
         continue;

      int fd = this->jobQueue.getJob();
      if (-1 == fd) // someone got the job before us
         continue;
      std::cout << "Thread " << std::this_thread::get_id() << ": Connected " << fd << "\n\n";

      // Overwriting the buffer: use the receive length to determine where the string terminates.
      // Must save the last byte for null-terminating character, hence the sizeof(buffer) - 1.
      receivedLen = recv(fd, buffer, sizeof(buffer) - 1, 0);
      buffer[receivedLen] = '\0';
      if (0 == receivedLen) {
         printf("Remote connection has been closed\n");
      }

      bufferStr = std::string(buffer);
      req.parseHeader(bufferStr);
      reqHeader = req.getHeader();

      // Validate header
      if (nullptr == reqHeader) {
         statusCode = StatusCode::BAD_REQUEST;
         errMsg = "Invalid header";
         goto SEND_RESPONSE;
      } else if (0 == reqHeader->size) {
         statusCode = StatusCode::PAYLOAD_TOO_LARGE;
         errMsg = "Request is too large";
         goto SEND_RESPONSE;
      } else if (!isHTTPVersionSupported(reqHeader->version)) {
         statusCode = StatusCode::HTTP_VERSION_NOT_SUPPORTED;
         errMsg = "Unsupported HTTP version";
         goto SEND_RESPONSE;
      }

      // Route the request to specified target
      resp = route(req);
      statusCode = resp.getStatusCode();

   SEND_RESPONSE:
      // send a response back to the server
      // log the errMsg
      std::string respStr = resp.toStr();
      send(fd, respStr.c_str(), respStr.length() + 1, 0);
      std::cout << respStr << "\n";
      std::cout << "Successfully processed request\n";
      shutdown(fd, 0);
      // if (200 <= static_cast<int>(statusCode) && static_cast<int>(statusCode) <= 299) {
      //    std::string respStr = resp.toStr();
      //    send(fd, respStr.c_str(), respStr.length() + 1, 0);
      //    std::cout << "Successfully processed request\n";
      // } else {
      //    std::cout << errMsg << "\n";
      // }
      // std::cout << "Thread " << std::this_thread::get_id() << ": Received " << buffer << "\n\n";
   }
   std::cout << "Thread " << std::this_thread::get_id() << " exiting....\n\n";
}

// Will overwrite previous handler if the target has already specified
void ServerModelHTTP::addRoute(std::string target, RequestMethod method, routeHandler handler) {
   switch (method) {
      case RequestMethod::GET:
         this->getRoutes[target] = handler;
         break;
      case RequestMethod::HEAD:
         break;
      case RequestMethod::POST:
         break;
      case RequestMethod::PUT:
         break;
      default:
         // should return error
         break;
   }
}

ResponseHTTP ServerModelHTTP::route(RequestHTTP &req) {
   ResponseHTTP response;
   RequestHeaderHTTP *header  = req.getHeader();
   std::string errMsg         = "";
   StatusCode statusCode      = StatusCode::OK;

   std::cout << "METHOD: " << (int)header->method << "\n";
   switch (header->method) {
      case RequestMethod::GET:
         std::cout << "TARGET: " << header->target << "\n";
         if (this->getRoutes.find(header->target) == this->getRoutes.end()) {
            errMsg = "Target is not found!";
            statusCode = StatusCode::NOT_FOUND;
            std::cout << "NOT FOUND!!!!\n";
            break;
         }
         break;
      case RequestMethod::HEAD:
         break;
      case RequestMethod::POST:
         break;
      case RequestMethod::PUT:
         break;
      default:
         // should return error
         break;
   }

   response.addStatusCode(statusCode);
   return response;
}

void ThreadSafeJobQueue::pushJob(int fd) {
   std::unique_lock<std::shared_mutex> lck(this->mu);
   this->jobs.push(fd);
}

int ThreadSafeJobQueue::getJob() {
   std::unique_lock<std::shared_mutex> lck(this->mu);
   // DO NOT CALL this->size()!!! Would result in a deadlock
   if (this->jobs.size() == 0)
      return -1;

   int job = this->jobs.front();
   this->jobs.pop();
   return job;
}

int ThreadSafeJobQueue::size() const {
   std::shared_lock lck(this->mu);
   return this->jobs.size();
}
