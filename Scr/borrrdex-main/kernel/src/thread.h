#pragma once

#ifndef __cplusplus
#error C++ Only
#endif

#include "cswitch.h"
#include <cstdint>

typedef int tid_t;

constexpr tid_t IDLE_THREAD_TID = 0;
constexpr uint8_t THREAD_FLAG_USERMODE = 0x1;
constexpr uint8_t THREAD_FLAG_ENTERUSER = 0x2;

const uint32_t THREAD_VIRTUAL_HEAP_START = 0x90000000;

typedef enum {
    THREAD_FREE,
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_SLEEPING,
    THREAD_NONREADY,
    THREAD_DYING
} thread_state_t;

typedef struct {
    context_t* context;
    context_t* user_context;
    thread_state_t state;
    uint32_t sleeps_on;
    void* pml4;
    tid_t next;
    uint32_t attribs;
    uint32_t padding[5];
} thread_table_t;

void thread_table_init();
tid_t thread_create(void(*func)(uint32_t), uint32_t arg);
void thread_run(tid_t t);

tid_t thread_get_current();
thread_table_t* thread_get_thread_entry(tid_t thread);
thread_table_t* thread_get_current_thread_entry();

void thread_switch();
void thread_yield();