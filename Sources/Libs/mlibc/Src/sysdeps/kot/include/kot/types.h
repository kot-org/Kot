#ifndef KOT_TYPES_H
#define KOT_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define KFAIL               0
#define KSUCCESS            1
#define KBUSY               2
#define KNOTALLOW           3
#define KKEYVIOLATION       4
#define KMEMORYVIOLATION    5

typedef uint32_t color_t;

typedef uint64_t size64_t;
typedef uint64_t KResult;

typedef uint64_t kot_key_t; 
typedef kot_key_t kot_process_t;
typedef kot_key_t kot_thread_t;
typedef kot_key_t kot_event_t;
typedef kot_key_t kot_key_mem_t;

typedef uint64_t kot_authorization_t;
typedef uint64_t kot_authorization_type_t;

typedef struct kot_arguments_t{
    uint64_t arg[6];
}__attribute__((packed));

typedef struct{
    uint32_t Data0:32;

    uint16_t Data1:16;
    uint16_t Data2:16;
    uint16_t Data3:16;

    uint64_t Data4:48;
}kot_GUID_t;

typedef struct{
    uint32_t Data0:32;

    uint16_t Data1:16;
    uint16_t Data2:16;
    uint16_t Data3:16;

    uint64_t Data4:48;
}kot_UUID_t;

// cyclic buffer
typedef struct{
    void* Base;
    size64_t Size;
}kot_cyclic_t;

typedef struct{
    void* Base;
    size64_t Size;
}kot_memory_buffer_t;

#if defined(__cplusplus)
} 
#endif

#endif