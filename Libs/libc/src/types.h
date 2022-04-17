#pragma once

#define NULL 0x0

#if !defined(__cplusplus)
#define bool unsigned int
#define true 1
#define false 0
#endif

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

#if __WORDSIZE == 64
typedef unsigned long int           size64_t;
#else
typedef unsigned long long int      size64_t;
#endif

#if __WORDSIZE == 64
typedef unsigned long int           KResult;
#else
typedef unsigned long long int      KResult;
#endif

#if __WORDSIZE == 64
typedef unsigned long int           kthread_t;
#else
typedef unsigned long long int      kthread_t;
#endif

typedef long unsigned int           size_t;
typedef void*                       uintptr_t;

#define KFAIL 0
#define KSUCCESS 1

struct parameters_t{
    uint64_t Parameter0;
    uint64_t Parameter1;
    uint64_t Parameter2;
    uint64_t Parameter3;
    uint64_t Parameter4;
    uint64_t Parameter5;
}__attribute__((packed));

extern uint64_t _main_thread;
extern uint64_t _process;