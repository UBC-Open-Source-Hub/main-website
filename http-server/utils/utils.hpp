#pragma once
#include <errno.h>
#include <cstring>
#include <string>

#include "responseHTTP.h"

#define eprintf(format, ...) \
   fprintf(stderr, format ": %s\n" __VA_OPT__(,) __VA_ARGS__, strerror(errno))

std::string getIpAddrStr(struct sockaddr *addr);

bool isHTTPVersionSupported(const std::string &version);

std::string getStatusCodeStr(StatusCode sc);

void blockSignal(int sig);
void unBlockSignal(int sig);
