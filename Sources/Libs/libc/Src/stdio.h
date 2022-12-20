#ifndef _STDIO_H
#define _STDIO_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

int sprintf(char *str, const char *format, ...);

#if defined(__cplusplus)
}
#endif

#endif