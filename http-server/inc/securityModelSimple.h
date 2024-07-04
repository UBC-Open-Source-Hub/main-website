#pragma once

#include "securityModel.h"

class SecurityModelSimple : public SecurityModel {
   public:
      SecurityModelSimple();
      ~SecurityModelSimple();

      void accept() override;

   private:
};
