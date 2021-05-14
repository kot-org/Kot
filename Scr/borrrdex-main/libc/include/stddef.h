#pragma once

#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void *)0)
#endif

typedef long double max_align_t;

#include "bits/alltypes.h"

#if __GNUC__ > 3
#define offsetof(type, member) __builtin_offsetof(type, member)
#else
#define offsetof(type, member) ((size_t)( (char *)&(((type *)0)->member) - (char *)0 ))
#endif