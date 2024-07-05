#pragma once

#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
class SecurityModel {
   public:
      SecurityModel(int socket);
      ~SecurityModel();

      // Pure virtual methods
      virtual int acceptConnections() = 0;

      virtual void processConnection(int fd) = 0;

   protected:
      int socket;
      bool isActive;
      std::unordered_map<std::thread::id, std::thread> workers;
      std::mutex mutex;
      // std::vector<std::thread> workers;
};
