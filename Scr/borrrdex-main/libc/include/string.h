#pragma once

#include "__config.h"
#include "stddef.h"

__BEGIN_DECLS

void* memcpy(void*, const void*, size_t);
int memcmp(const void*, const void*, size_t) __attribute__((pure));
void memset(void*, int, size_t);
void* memmove(void*, const void*, size_t);

size_t strnlen(const char*, size_t);
char *strncpy (char *__restrict, const char *__restrict, size_t);
int strncmp (const char *, const char *, size_t) __attribute__((pure));

__END_DECLS