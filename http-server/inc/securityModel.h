#pragma once
class SecurityModel {
   public:
      SecurityModel();
      ~SecurityModel();

      // Pure virtual methods
      virtual void accept() = 0;

   private:
};
