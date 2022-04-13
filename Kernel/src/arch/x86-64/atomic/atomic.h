#pragma once
#include <lib/types.h>
#include <arch/arch.h>

namespace Atomic{
    extern "C" bool atomicLock(uint64_t* mutex, uint64_t bit);
    extern "C" bool atomicUnlock(uint64_t* mutex, uint64_t bit);
    extern "C" void atomicSpinlock(uint64_t* mutex, uint64_t bit);
}