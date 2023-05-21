#pragma once
#include <lib/types.h>

typedef struct locker_t{
    uint64_t Mutex;
};

extern "C" bool AtomicClearLock(locker_t* locker);
extern "C" bool AtomicLock(locker_t* locker);
extern "C" bool AtomicRelease(locker_t* locker);
extern "C" void AtomicAquire(locker_t* locker);
extern "C" void AtomicAquireCli(locker_t* locker);