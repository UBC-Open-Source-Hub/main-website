#pragma once

#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
class SecurityModel {
   public:
      SecurityModel(): socket(0), pipeFds{0, 0}, isActive(false) {}

      ~SecurityModel() {}

      // Pure virtual methods
      virtual int acceptConnections(int socket) = 0;

      virtual void processConnection(int fd) = 0;

      virtual void deactivate() volatile = 0;

   protected:
      virtual void shutdownConnections() = 0;

      int socket;
      int pipeFds[2];
      bool isActive;
      std::unordered_map<std::thread::id, std::pair<std::thread, int>> workers;
      std::mutex mutex;
};
