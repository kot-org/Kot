#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include "paging/PageTableManager.h"
#include <cstdint>

typedef struct {
    uint64_t* stack;
    uint64_t rip;
    uint64_t flags;
    uint64_t pml4;
    uint64_t* virt_memory;
    void* prev_context;
} context_t;

void context_init(context_t* ctx, uint64_t entry, uint64_t endentry, uint64_t stack, uint32_t args);
void context_enter_userland(context_t* ctx);
void context_enable_ints(context_t* ctx);
void context_set_sp(context_t* ctx, uint64_t sp);
void context_set_ip(context_t* ctx, uint64_t ip);

void cswitch_vector_code();