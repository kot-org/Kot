#ifndef _LOCK_H
#define _LOCK_H 1

#include <stdbool.h>
#include <stddef.h>

#define SPINLOCK_INIT {0, NULL}

typedef struct {
    int lock;
    void *last_acquirer;
} spinlock_t;

static inline bool spinlock_test_and_acq(spinlock_t *lock) {
    return __sync_bool_compare_and_swap(&lock->lock, 0, 1);
}

void spinlock_acquire(spinlock_t *lock);

static inline void spinlock_release(spinlock_t *lock) {
    lock->last_acquirer = NULL;
    __atomic_store_n(&lock->lock, 0, __ATOMIC_SEQ_CST);
}

#endif // _LOCK_H
