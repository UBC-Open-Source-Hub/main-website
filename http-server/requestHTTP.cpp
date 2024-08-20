#include "inc/requestHTTP.h"

RequestHeaderHTTP* RequestHTTP::parseHeader(const std::string &content) {
   return nullptr;
}

RequestHTTP::RequestHTTP(const std::string &content, RequestHeaderHTTP *header) {
   if (nullptr == header)
      this->header = RequestHTTP::parseHeader(content);

   if (nullptr == this->header || 0 == this->header->size)
      throw "HTTP Request is not valid: Incomplete or Invalid header";

   this->body = content.substr(this->header->size + 2);
}
