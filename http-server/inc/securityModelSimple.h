#pragma once

#include "securityModel.h"

class SecurityModelSimple : public SecurityModel {
   public:
      SecurityModelSimple(): SecurityModel() {};
      ~SecurityModelSimple() {};

      int acceptConnections(int socket) override;
      void processConnection(int fd) override;
      void deactivate() volatile override;
   
   protected:
      void shutdownConnections() override;

   private:
};
