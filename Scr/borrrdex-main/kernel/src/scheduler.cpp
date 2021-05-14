#include "scheduler.h"
#include "spinlock.h"
#include "config.h"
#include "thread.h"
#include "Panic.h"
#include "arch/x86_64/interrupt/interrupt.h"

extern Spinlock thread_table_spinlock;
extern thread_table_t thread_table[CONFIG_MAX_THREADS];

tid_t scheduler_current_thread[CONFIG_MAX_CPUS];

static struct {
    tid_t head;
    tid_t tail;
} ready_to_run = { -1, -1 };

static void add_to_ready_list(tid_t t) {
    KERNEL_ASSERT(t != IDLE_THREAD_TID);
    KERNEL_ASSERT(t >=0 && t < CONFIG_MAX_THREADS);

    if(ready_to_run.tail < 0) {
        /* ready queue was empty */
        ready_to_run.head = t;
        ready_to_run.tail = t;
        thread_table[t].next = -1;
    } else {
        thread_table[ready_to_run.tail].next = t;
        thread_table[t].next = -1;
        ready_to_run.tail = t;
    }
}

static tid_t remove_first_ready() {
    tid_t t = ready_to_run.head;
    KERNEL_ASSERT(t != IDLE_THREAD_TID);

    if(t >= 0) {
        KERNEL_ASSERT(thread_table[t].state == THREAD_READY);
        if(ready_to_run.tail == t) {
            ready_to_run.tail = -1;
        }

        ready_to_run.head = thread_table[ready_to_run.head].next;
    }

    if(t < 0) {
        return IDLE_THREAD_TID;
    }

    return t;
}

void scheduler_init() {
    for(int i = 0; i < CONFIG_MAX_CPUS; i++) {
        scheduler_current_thread[i] = 0;
    }
}

void scheduler_add_ready(tid_t t) {
    WithInterrupts wi(false);

    thread_table_spinlock.acquire();
    
    add_to_ready_list(t);
    thread_table[t].state = THREAD_READY;

    thread_table_spinlock.release();
}

void scheduler_mark_finished() {
    scheduler_current_thread[interrupt_get_cpu()] = IDLE_THREAD_TID;
}

void scheduler_schedule() {
    int this_cpu = interrupt_get_cpu();
    thread_table_spinlock.acquire();

    thread_table_t* current_thread = &(thread_table[scheduler_current_thread[this_cpu]]);
    if(current_thread->state == THREAD_DYING) {
        current_thread->state = THREAD_FREE;
    } else if(current_thread->sleeps_on != 0) {
        current_thread->state = THREAD_SLEEPING;
    } else {
        if(scheduler_current_thread[this_cpu] != IDLE_THREAD_TID) {
            add_to_ready_list(scheduler_current_thread[this_cpu]);
        }

        current_thread->state = THREAD_READY;
    }

    tid_t t = remove_first_ready();
    thread_table[t].state = THREAD_RUNNING;

    thread_table_spinlock.release();

    scheduler_current_thread[this_cpu] = t;

}