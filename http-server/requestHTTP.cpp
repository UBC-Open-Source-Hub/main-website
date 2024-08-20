#include "inc/requestHTTP.h"
#include <sstream>
#include <string>

RequestHeaderHTTP* RequestHTTP::parseHeader(const std::string &content) {
   std::stringstream ssLine(content);
   std::string line;

   int numLines   = 0;
   int headerSize = 0;

   RequestHeaderHTTP *newHeader = new RequestHeaderHTTP();

   std::getline(ssLine, line);
   headerSize += line.length();
   numLines++;

   parseStartLine(line, newHeader);
   if (RequestMethod::UNSUPPORTED == newHeader->method ||
       "" == newHeader->target ||
       "" == newHeader->version)
      goto ERROR;
   
   while(std::getline(ssLine, line)) {
      std::stringstream ssToken(line);
      std::string token;

      headerSize += line.length();
      numLines++;

      // Found the separation line
      if ("\r" == line) {
          // must also account for all stripped new lines
         newHeader->size = headerSize + numLines;
         break;
      }

      // The first token of the line will be the header name
      std::string fieldName = "";
      while (std::getline(ssToken, token, ' ')) {
         if ("" == token) {
            if ("" != fieldName)
               newHeader->fields[fieldName] += " ";
            continue;
         }

         // Strip ending return-carriage ('\r')
         if ('\r' == token[token.length() - 1])
            token.pop_back();

         if ("" == fieldName) {
            if (token.length() == 0 || token[token.length() - 1] != ':')
               goto ERROR;
            token.pop_back();
            fieldName = token;
         } else {
            newHeader->fields[fieldName] += " " + token;
         }
      }
   }

NORMAL_RETURN:
   return newHeader;

ERROR:
   delete newHeader;
   newHeader = nullptr;
   goto NORMAL_RETURN;
}

void RequestHTTP::parseStartLine(const std::string &startLine, RequestHeaderHTTP *req) {
   std::stringstream ssLine(startLine);
   std::string token;

   // Extract method
   ssLine >> token;
   if ("GET" == token)
      req->method = RequestMethod::GET;
   else if ("HEAD" == token)
      req->method = RequestMethod::HEAD;
   else
      req->method = RequestMethod::UNSUPPORTED;
   
   ssLine >> req->target;
   ssLine >> req->version;
}

RequestHTTP::RequestHTTP(const std::string &content, RequestHeaderHTTP *header) {
   if (nullptr == header)
      this->header = RequestHTTP::parseHeader(content);

   if (nullptr == this->header || 0 == this->header->size)
      throw "HTTP Request is not valid: Incomplete or Invalid header";

   this->body = content.substr(this->header->size + 2);
}
