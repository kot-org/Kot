#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include "config.h"
#include "thread.h"
#include "spinlock.h"

class Semaphore {
public:
    static Semaphore* create(int value);
    void free();

    void increment();
    void decrement();
private:
    static Semaphore semaphore_table[CONFIG_MAX_SEMAPHORES];
    
    Semaphore(){}
    tid_t _creator;
    int _value;
    Spinlock _slock;
};