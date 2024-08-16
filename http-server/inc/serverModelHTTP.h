#pragma once

#include "serverModel.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <semaphore>
#include <shared_mutex>
#include <stdatomic.h>
#include <thread>
#include <vector>

// number of workers on standby
#define NUM_WORKERS 5 // should be increased to a much larger number later
#define MAX_NUM_WORKERS 20

// thread-safe job queue
class ThreadSafeJobQueue {
   public:
      ThreadSafeJobQueue() {}
      ~ThreadSafeJobQueue() {}

      // Add job to the queue
      void pushJob(int fd);

      // Get job in front of the line. Also remove the job
      // Return -1 if jobs.size() == 0
      int getJob();

      int size() const;

   private:
      std::queue<int> jobs;
      mutable std::shared_mutex mu;
};

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
      int pipeFds[2];

      std::vector<std::thread*> workers;
      ThreadSafeJobQueue jobQueue;
      std::counting_semaphore<MAX_NUM_WORKERS> sem;
};
