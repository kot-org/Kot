#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Atomic{

    extern "C" void atomicLock(void* mutex);
    extern "C" void atomicUnlock(void* mutex);
    extern "C" void atomicWait(void* mutex);
}