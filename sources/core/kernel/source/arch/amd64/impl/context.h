#ifndef _AMD64_IMPL_CONTEXT_H
#define _AMD64_IMPL_CONTEXT_H

#include <impl/arch.h>

typedef struct{
    /* mandatory fields for every arch */
    cpu_context_t cpu_ctx;
    /* optional fields */
    void* simd_ctx;
    uint64_t fs_base;
}context_t;

#endif