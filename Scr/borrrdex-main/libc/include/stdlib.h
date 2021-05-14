#pragma once

#include "__config.h"
#include "features.h"

#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void*)0)
#endif

#include "bits/alltypes.h"

__BEGIN_DECLS

int atoi(const char *);
long atol(const char *);
long long atoll(const char *);

void* calloc(size_t, size_t);

_Noreturn void exit(int);

void* malloc(size_t);

void* realloc(void*, size_t);

__END_DECLS