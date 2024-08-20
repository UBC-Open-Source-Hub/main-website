#pragma once

#include <cstdint>
enum struct StatusCode : uint16_t {
   // 2xx Status Codes
   OK = 200,

   // 4xx Status codes
   BAD_REQUEST = 400,
   PAYLOAD_TOO_LARGE = 413,

   // 5xx Status Codes
   NOT_IMPLEMENTED = 501,
   HTTP_VERSION_NOT_SUPPORTED = 505,
};
