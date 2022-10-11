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
typedef key_t process_t;
typedef key_t thread_t;
typedef key_t event_t;
typedef key_t ksmem_t;

typedef void*                       uintptr_t;

typedef long unsigned int           size_t;
typedef uint64_t                    size64_t;

#define KFAIL               0
#define KSUCCESS            1
#define KBUSY               2
#define KNOTALLOW           3
#define KKEYVIOLATION       4
#define KMEMORYVIOLATION    5

#if !defined(__cplusplus)
#define bool uint8_t
#define true 1
#define false 0
#endif

typedef struct arguments_t{
    uint64_t arg[6];
}__attribute__((packed));

typedef struct GUID_t{
    uint32_t Data0;

    uint16_t Data1;
    uint16_t Data2;

    uint8_t Data4[8];
};

#endif
