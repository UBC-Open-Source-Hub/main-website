#pragma once

#include "serverModel.h"

class ServerModelHTTP : public ServerModel {
   public:
      ServerModelHTTP() : ServerModel() {} ;
      ~ServerModelHTTP() {} ;

      int acceptConnections(int socket) override;

      void processConnection(int fd) override;

      void deactivate() volatile override;

   protected:
      void shutdownConnections() override;

   private:
};
