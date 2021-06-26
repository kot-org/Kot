#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../paging/pageFrameAllocator.h"

namespace Atomic{
    void* atomicLoker(void* mutex);
    extern "C" void atomicLock(void* mutex);
    extern "C" void atomicUnlock(void* mutex);
    extern "C" void atomicWait(void* mutex);
    extern "C" bool atomicCheck(void* mutex);
}