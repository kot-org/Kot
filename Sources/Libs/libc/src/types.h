#ifndef _TYPES_H
#define _TYPES_H 1

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

typedef uint64_t size64_t;
typedef uint64_t KResult;

/* Keys */
typedef uint64_t key_t; 
typedef key_t kprocess_t;
typedef key_t kthread_t;
typedef key_t kevent_t;
typedef key_t ksmem_t;

typedef void*                       uintptr_t;

typedef long unsigned int           size_t;

#define KFAIL               0
#define KSUCCESS            1
#define KNOTALLOW           2
#define KKEYVIOLATION       3
#define KMEMORYVIOLATION    4

#if !defined(__cplusplus)
#define bool uint64_t
#define true 1
#define false 0
#endif

struct parameters_t{
    uint64_t Parameter0;
    uint64_t Parameter1;
    uint64_t Parameter2;
    uint64_t Parameter3;
    uint64_t Parameter4;
    uint64_t Parameter5;
}__attribute__((packed));

#endif