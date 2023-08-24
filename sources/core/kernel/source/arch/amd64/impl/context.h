#ifndef _AMD64_IMPL_CONTEXT_H
#define _AMD64_IMPL_CONTEXT_H

#include <impl/arch.h>

typedef struct{
    cpu_context_t cpu_ctx;
    void* simd_ctx;
}context_t;

#endif