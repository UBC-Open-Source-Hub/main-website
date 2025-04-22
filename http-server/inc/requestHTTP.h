#pragma once

#include <string>
#include <unordered_map>

#define HEADER_MAX_SIZE 8192

enum struct RequestMethod {
   UNSUPPORTED = 0,
   GET,
   HEAD,
   POST,
   PUT
};

struct RequestHeaderHTTP {
   // Header attributes/metadata
   int size;
    
   // Request line
   RequestMethod method;
   std::string target;
   std::string version;
   
   // The rest of the headers
   std::unordered_map<std::string, std::string> fields;
};

class RequestHTTP {
   public:
      RequestHTTP() {};
      ~RequestHTTP();
      
      // Return nullptr if:
      // - the header is not valid
      // Return RequestHeaderHTTP of size 0 if:
      // - no separation line (\r\n) are found. This could be due to incomplete header
      void parseHeader(const std::string &content);
 
      // Return the HTTP Header
      RequestHeaderHTTP* getHeader();

   private:
      void parseStartLine(const std::string &startLine, RequestHeaderHTTP *req);

      RequestHeaderHTTP *header;
      std::string body;
};
