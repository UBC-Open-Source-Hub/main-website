#pragma once

#include "securityModel.h"

class SecurityModelSimple : public SecurityModel {
   public:
      SecurityModelSimple(int socket);
      ~SecurityModelSimple();

      int acceptConnections() override;

   private:
};
