#include "inc/responseHTTP.h"
#include "inc/utils.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

void ResponseHTTP::addStatusCode(StatusCode sc) {
   this->statusCode = sc;
}

StatusCode ResponseHTTP::getStatusCode() {
   return this->statusCode;
}

void ResponseHTTP::addBody(std::string body) {
   this->body = body;
}

std::string ResponseHTTP::toStr() {
   // Build header
   std::stringstream ss;
   ss << PROTOCOL << " " << (int)this->statusCode << " " << getStatusCodeStr(this->statusCode) << "\r\n";
   ss << CONTENT_TYPE << ": text/html" << "\n";
   // ss << CONNECTION << ": keep-alive" << "\n";
   // ss << KEEP_ALIVE << ": timeout=5" << "\n";
   time_t currTime = time(nullptr);
   // ss << DATE << ": " << ctime(&currTime) << "\n";

   // Separation line
   // ss << "\n";
   //
   // ss << "<!DOCTYPE html><html lang=\"en\"></html>";

   // Build body
   return ss.str();
}
