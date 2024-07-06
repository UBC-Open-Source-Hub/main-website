#pragma once

// Abstract class that any security model must implement
class SecurityModel {
   public:
      SecurityModel(): socket(0), isActive(false) {}

      ~SecurityModel() {}

      // Pure virtual methods
      // Begin accepting new connections using given socket's file descriptor
      virtual int acceptConnections(int socket) = 0;

      // Process the connection
      virtual void processConnection(int fd) = 0;

      // Deactivate the model. This is usually called upon server's shutdown
      virtual void deactivate() volatile = 0;

   protected:
      virtual void shutdownConnections() = 0;

      int socket;
      bool isActive;
};
