#pragma once

#include <cstdint>
#include <string>

#define PROTOCOL "HTTP/1.1"
#define CONTENT_TYPE "Content-Type"
#define CONNECTION "Connection"
#define KEEP_ALIVE "Keep-Alive"
#define DATE "Date"

enum struct StatusCode : uint16_t {
   // 2xx Status Codes
   OK = 200,

   // 4xx Status codes
   BAD_REQUEST = 400,
   NOT_FOUND   = 404,
   PAYLOAD_TOO_LARGE = 413,

   // 5xx Status Codes
   NOT_IMPLEMENTED = 501,
   HTTP_VERSION_NOT_SUPPORTED = 505,
};

class ResponseHTTP {
   public:
      ResponseHTTP () {};
      ~ResponseHTTP () {};

      void addStatusCode(StatusCode sc);
      StatusCode getStatusCode();

      void addBody(std::string body);

      std::string toStr();

   private:
      StatusCode  statusCode;
      std::string body;
};
// class Respons {
//    public:
//       // Return nullptr if:
//       // - the header is not valid
//       // Return RequestHeaderHTTP of size 0 if:
//       // - no separation line (\r\n) are found. This could be due to incomplete header
//       static RequestHeaderHTTP* parseHeader(const std::string &content);
//
//       RequestHTTP() = delete;
//       RequestHTTP(const std::string &content, RequestHeaderHTTP *header = nullptr);
//       ~RequestHTTP();
//
//    private:
//       static void parseStartLine(const std::string &startLine, RequestHeaderHTTP *req);
//       RequestHeaderHTTP *header;
//       std::string body;
// };
