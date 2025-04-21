#include "inc/requestHTTP.h"
#include <sstream>
#include <string>

void RequestHTTP::parseHeader(const std::string &content) {
  std::stringstream ssLine(content);
  std::string line;

  int numLines = 0;
  int headerSize = 0;

  header = new RequestHeaderHTTP();

  std::getline(ssLine, line);
  headerSize += line.length();
  numLines++;

  parseStartLine(line, header);
  if (RequestMethod::UNSUPPORTED == header->method || 
      "" == header->target ||
      "" == header->version)
    return;

  while (std::getline(ssLine, line)) {
    std::stringstream ssToken(line);
    std::string token;

    headerSize += line.length();
    numLines++;

    // Found the separation line
    if ("\r" == line) {
      // must also account for all stripped new lines
      header->size = headerSize + numLines;
      break;
    }

    // The first token of the line will be the header name
    std::string fieldName = "";
    while (std::getline(ssToken, token, ' ')) {
      if ("" == token) {
        if ("" != fieldName)
          header->fields[fieldName] += " ";
        continue;
      }

      // Strip ending return-carriage ('\r')
      if ('\r' == token[token.length() - 1])
        token.pop_back();

      if ("" == fieldName) {
        if (token.length() == 0 || token[token.length() - 1] != ':')
          return;
        token.pop_back();
        fieldName = token;
      } else {
        header->fields[fieldName] += " " + token;
      }
    }
  }
}

void RequestHTTP::parseStartLine(const std::string &startLine,
                                 RequestHeaderHTTP *req) {
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

RequestHTTP::~RequestHTTP() {
  if (this->header)
    delete this->header;
}

RequestHeaderHTTP* RequestHTTP::getHeader() {
   return this->header;
}
