#pragma once
#include <kot/types.h>

typedef struct locker_t{
    uint64_t Mutex;
};

extern "C" bool AtomicLock(locker_t* locker);
extern "C" bool AtomicRelease(locker_t* locker);
extern "C" void AtomicAquire(locker_t* locker);