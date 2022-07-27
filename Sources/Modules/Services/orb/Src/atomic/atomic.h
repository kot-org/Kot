#pragma once
#include <kot/types.h>
#include <kot/arch.h>

namespace Atomic{
    extern "C" bool atomicLock(uint64_t* mutex, uint64_t bit);
    extern "C" bool atomicUnlock(uint64_t* mutex, uint64_t bit);
    extern "C" void atomicAcquire(uint64_t* mutex, uint64_t bit);
}