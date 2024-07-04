#pragma once
class SecurityModel {
   public:
      SecurityModel(int socket);
      ~SecurityModel();

      // Pure virtual methods
      virtual int acceptConnections() = 0;

   protected:
      int socket;
      bool isActive;
};
