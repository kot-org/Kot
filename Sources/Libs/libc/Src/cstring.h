#ifndef _CSTRING_H
#define _CSTRING_H 1

#include <kot/sys.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

int64_t asi(int64_t n, int basenumber);
int strlen(char* p);
int strcmp(char* a, char* b);
int strncmp(char* a, char* b, size64_t len);
void strcpy(char* to, char* from);
char* strcat(char* dst, char* src);
int strtol(const char* str, char** endptr, int8_t basenumber);
char** strsplit(char* str, const char* delimiters, uint64_t* count);
void freeSplit(char** splitData);
int atoi(const char* str);
char* itoa(int64_t value, char* buffer, int base);

#if defined(__cplusplus)
}
#endif

#endif