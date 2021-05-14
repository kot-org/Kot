#pragma once

#include "stdint.h"
#include "stddef.h"

#define _Addr int

typedef ptrdiff_t ssize_t;

typedef __builtin_va_list va_list;
typedef __builtin_va_list __isoc_va_list;

typedef int64_t off_t;
typedef long time_t;
typedef long suseconds_t;

struct timeval { time_t tv_sec; suseconds_t tv_usec; };
struct timespec { time_t tv_sec; long tv_nsec; };

typedef float float_t;
typedef double double_t;