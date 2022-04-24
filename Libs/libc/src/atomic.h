#ifndef _ATOMIC_H
#define _ATOMIC_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

bool atomicLock(uint64_t* mutex, uint64_t bit);
bool atomicUnlock(uint64_t* mutex, uint64_t bit);
void atomicSpinlock(uint64_t* mutex, uint64_t bit);

#if defined(__cplusplus)
}
#endif

#endif