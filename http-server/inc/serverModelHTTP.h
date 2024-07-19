#pragma once

#include "serverModel.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdatomic.h>
#include <thread>
#include <vector>

// number of workers on standby
#define NUM_WORKERS 5 // should be increased to a much larger number later

class ServerModelHTTP : public ServerModel {
   public:
      ServerModelHTTP();
      ~ServerModelHTTP() override;

      int acceptConnections(int socket) override;

      void processConnection(int fd) override;

      void deactivate() volatile override;

      void startWorker();

   protected:
      void shutdownConnections() override;

   private:
      std::mutex mu;
      std::condition_variable condVar;
      std::vector<std::thread*> workers;
      std::queue<int> jobs;
};
