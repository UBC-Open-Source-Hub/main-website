#pragma once

#include "securityModel.h"
#include <unordered_map>
#include <thread>

// Simple security model that acts as an "echo" server
class SecurityModelSimple : public SecurityModel {
   public:
      SecurityModelSimple(): SecurityModel(), pipeFds{0, 0}  {};
      ~SecurityModelSimple() {};

      int acceptConnections(int socket) override;
      void processConnection(int fd) override;
      void deactivate() volatile override;
   
   protected:
      void shutdownConnections() override;

   private:
      int pipeFds[2];

      std::unordered_map<std::thread::id, std::pair<std::thread, int>> workers;
      std::mutex mutex;
};
