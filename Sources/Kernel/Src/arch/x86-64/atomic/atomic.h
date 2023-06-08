#pragma once
#include <lib/types.h>

typedef struct locker_t{
    uint64_t Mutex;
};

extern "C" bool AtomicClearLock(locker_t* locker);
extern "C" bool AtomicLock(locker_t* locker);
extern "C" bool AtomicRelease(locker_t* locker);
extern "C" void AtomicAcquire(locker_t* locker);
extern "C" void AtomicAcquireCli(locker_t* locker);