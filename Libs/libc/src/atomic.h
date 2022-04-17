#pragma once
#include <kot/types.h>

bool atomicLock(uint64_t* mutex, uint64_t bit);
bool atomicUnlock(uint64_t* mutex, uint64_t bit);
void atomicSpinlock(uint64_t* mutex, uint64_t bit);