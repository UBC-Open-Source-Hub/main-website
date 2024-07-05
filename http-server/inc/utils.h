#pragma once
#include <errno.h>
#include <cstring>

#define eprintf(format, ...) \
   fprintf(stderr, format ": %s\n" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
