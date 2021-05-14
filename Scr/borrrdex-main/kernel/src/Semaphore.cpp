#include "Semaphore.h"
#include "Panic.h"
#include "arch/x86_64/interrupt/interrupt.h"

static Spinlock semaphore_table_lock;
static bool initialized = false;

Semaphore Semaphore::semaphore_table[];

Semaphore* Semaphore::create(int value) {
    KERNEL_ASSERT(value >= 0);
    static int next = 0;
    int i;
    int sem_id;

    {
        WithInterrupts wi(true);

        semaphore_table_lock.acquire();
        if(!initialized) {
            initialized = true;
            for(int i = 0; i < CONFIG_MAX_SEMAPHORES; i++) {
                semaphore_table[i]._creator = -1;
            }
        }

        for(i = 0; i < CONFIG_MAX_SEMAPHORES; i++) {
            sem_id = next;
            next = (next + 1) % CONFIG_MAX_SEMAPHORES;
            if(semaphore_table[sem_id]._creator == -1) {
                semaphore_table[sem_id]._creator = thread_get_current();
                break;
            }
        }

        semaphore_table_lock.release();
    }

    if(i == CONFIG_MAX_SEMAPHORES) {
        return nullptr;
    }

    semaphore_table[sem_id]._value = value;
    semaphore_table[sem_id]._slock.release();
    return &semaphore_table[sem_id];
}

void Semaphore::free() {
    _creator = -1;
}

void Semaphore::increment() {
    WithInterrupts wi(false);
    _slock.acquire();

    _value++;
    if(_value <= 0) {

    }

    _slock.release();
}

void Semaphore::decrement() {
    WithInterrupts wi(false);
    _slock.acquire();

    _value--;
    if(_value < 0) {
        _slock.release();
        thread_switch();
    } else {
        _slock.release();
    }
}