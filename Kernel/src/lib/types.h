#pragma once

#define NULL 0x0

typedef signed char                 int8_t;
typedef short int                   int16_t;
typedef int                         int32_t;

#if __WORDSIZE == 64
typedef long int                    int64_t;
#else
typedef long long int               int64_t;
#endif

/* Unsigned.  */
typedef unsigned char               uint8_t;
typedef unsigned short int          uint16_t;
typedef unsigned int                uint32_t;

#if __WORDSIZE == 64
typedef unsigned long int           uint64_t;
#else
typedef unsigned long long int      uint64_t;
#endif

typedef long unsigned int           size_t;
typedef void*                       uintptr_t;