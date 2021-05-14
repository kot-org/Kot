#include "thread.h"
#include "Panic.h"
#include "spinlock.h"
#include "config.h"
#include "scheduler.h"
#include "Panic.h"
#include "KernelUtil.h"
#include "arch/x86_64/interrupt/interrupt.h"

#include <cstddef>

extern "C" void __idle_thread_wait_loop();

// External for scheduler
Spinlock thread_table_spinlock;
thread_table_t thread_table[CONFIG_MAX_THREADS];

extern tid_t scheduler_current_thread[CONFIG_MAX_CPUS];

char thread_stack_areas[CONFIG_THREAD_STACKSIZE * CONFIG_MAX_THREADS];

__attribute__((noreturn)) static void thread_finish() {
    tid_t my_tid = thread_get_current();

    interrupt_disable();
    KERNEL_ASSERT(thread_table[my_tid].pml4 == nullptr);

    thread_table_spinlock.acquire();
    thread_table[my_tid].state = THREAD_DYING;
    thread_table_spinlock.release();

    interrupt_enable();
    interrupt_yield();

    Panic("thread_finish(): thread was not destroyed");
    __builtin_unreachable();
}

void thread_table_init() {
    KERNEL_ASSERT(sizeof(thread_table_t) == 64);

    for(int i = 0; i < CONFIG_MAX_THREADS; i++) {
        thread_table[i].context = (context_t *)(thread_stack_areas + CONFIG_THREAD_STACKSIZE * i + CONFIG_THREAD_STACKSIZE - sizeof(context_t));
        thread_table[i].user_context = NULL;
        thread_table[i].state = THREAD_FREE;
        thread_table[i].sleeps_on = 0;
        thread_table[i].pml4 = NULL;
        thread_table[i].attribs      = 0;
        thread_table[i].next         = -1;
    }

    context_set_ip(thread_table[IDLE_THREAD_TID].context, (uint64_t)__idle_thread_wait_loop);
    context_set_sp(thread_table[IDLE_THREAD_TID].context, (uint64_t)thread_stack_areas + CONFIG_THREAD_STACKSIZE - 4 - sizeof(context_t));
    context_enable_ints(thread_table[IDLE_THREAD_TID].context);
    thread_table[IDLE_THREAD_TID].context->pml4 = (uint64_t)KernelPageTableManager()->PML4Address();
    thread_table[IDLE_THREAD_TID].state = THREAD_READY;
    thread_table[IDLE_THREAD_TID].context->prev_context = thread_table[IDLE_THREAD_TID].context;
}

tid_t thread_create(void(*func)(uint32_t), uint32_t arg) {
    static tid_t next_tid = 0;
    tid_t tid = -1;
    
    interrupt_status_t intr_status = interrupt_disable();
    thread_table_spinlock.acquire();

    for(tid_t i = 0; i < CONFIG_MAX_THREADS; i++) {
        tid_t t = (i + next_tid) % CONFIG_MAX_THREADS;
        if(t == IDLE_THREAD_TID) {
            continue;
        }

        if(thread_table[t].state == THREAD_FREE) {
            tid = t;
            break;
        }
    }

    if(tid < 0) {
        thread_table_spinlock.release();
        interrupt_set_state(intr_status);
        return tid;
    }

    next_tid = (tid + 1) % CONFIG_MAX_THREADS;
    thread_table[tid].state = THREAD_NONREADY;
    thread_table_spinlock.release();

    thread_table[tid].context = (context_t *)(thread_stack_areas + CONFIG_THREAD_STACKSIZE * tid + CONFIG_THREAD_STACKSIZE - sizeof(context_t));
    for(int i = 0; i < sizeof(context_t) / 4; i++) {
        *(((uint64_t *)thread_table[tid].context) + i) = 0;
    }

    thread_table[tid].user_context = nullptr;
    thread_table[tid].pml4 = nullptr;
    thread_table[tid].sleeps_on = 0;
    thread_table[tid].attribs = 0;
    thread_table[tid].next = -1;

    thread_table[tid].context->prev_context = thread_table[tid].context;
    context_init(thread_table[tid].context, (uint64_t)func, (uint64_t)thread_finish, (uint64_t)thread_stack_areas + (CONFIG_THREAD_STACKSIZE * tid)
                + CONFIG_THREAD_STACKSIZE - 4 - sizeof(context_t), arg);

    return tid;
}

void thread_run(tid_t t) {
    scheduler_add_ready(t);
}

tid_t thread_get_current() {
    WithInterrupts wi(false);
    return scheduler_current_thread[interrupt_get_cpu()];
}

thread_table_t* thread_get_thread_entry(tid_t thread) {
    return &thread_table[thread];
}

thread_table_t* thread_get_current_thread_entry() {
    WithInterrupts wi(false);
    tid_t t = scheduler_current_thread[interrupt_get_cpu()];
    return thread_get_thread_entry(t);
}

void thread_switch() {
    WithInterrupts wi(true);
    interrupt_yield();
}

