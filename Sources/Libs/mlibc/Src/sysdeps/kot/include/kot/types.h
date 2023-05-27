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

typedef struct kot_GUID_t{
    uint32_t Data0;

    uint16_t Data1;
    uint16_t Data2;
    uint16_t Data3;

    uint8_t Data4[6];
}__attribute__((packed));

typedef struct kot_UUID_t{
    uint32_t Data0;

    uint16_t Data1;
    uint16_t Data2;
    uint16_t Data3;

    uint8_t Data4[6];
}__attribute__((packed));

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