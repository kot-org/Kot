#ifndef _CSTRING_H
#define _CSTRING_H 1

#include <kot/sys.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

int strlen(char* p);
int strcmp(char* a, char* b);
int strncmp(char* a, char* b, size_t len);

#if defined(__cplusplus)
}
#endif

#endif