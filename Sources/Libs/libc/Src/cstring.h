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
char* strcat(char* dst, const char* src);
char** strsplit(char* str, const char* delimiters);
void freeSplit(char** splitData);
int atoi(const char* str);
char* itoa(int64_t value, char* buffer, int base);
void strcpy(char* to, char* from);

#if defined(__cplusplus)
}
#endif

#endif