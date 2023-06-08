#ifndef KOT_CSTRING_H
#define KOT_CSTRING_H 1

#include <kot/sys.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

int64_t asi(int64_t n, int basenumber);
char* itoa(int64_t value, char* buffer, int base);
char** strsplit(char* str, char* delimiters, uint64_t* count);
void freestrsplit(char** splitData);

#if defined(__cplusplus)
}
#endif

#endif
