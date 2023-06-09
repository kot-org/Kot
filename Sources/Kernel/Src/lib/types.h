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

#if defined(__LP64__) || defined(_LP64)
typedef uint64_t uintptr_t;
#else
typedef uint32_t uintptr_t;
#endif

typedef uint32_t color_t;

typedef uint64_t size64_t;
typedef uint64_t KResult;

/* Keys */
typedef uint64_t key_t; 
typedef key_t process_t;
typedef key_t thread_t;
typedef key_t kot_event_t;
typedef key_t kot_key_mem_t;

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

typedef __builtin_va_list	va_list;
#define va_start(v,l)		__builtin_va_start(v,l)
#define va_end(v)			__builtin_va_end(v)
#define va_arg(v,l)			__builtin_va_arg(v,l)
#define va_copy(d,s)		__builtin_va_copy(d,s)

typedef struct arguments_t{
    uint64_t arg[6];
}__attribute__((packed));

typedef struct{
    uint32_t Data0:32;

    uint16_t Data1:16;
    uint16_t Data2:16;
    uint16_t Data3:16;

    uint64_t Data4:48;
}GUID_t;

typedef struct{
    uint32_t Data0:32;

    uint16_t Data1:16;
    uint16_t Data2:16;
    uint16_t Data3:16;

    uint64_t Data4:48;
}UUID_t;

// cyclic buffer
typedef struct{
    void* Base;
    size64_t Size;
}cyclic_t;

typedef struct{
    void* Base;
    size64_t Size;
}memory_buffer_t;

typedef void* pagetable_t;