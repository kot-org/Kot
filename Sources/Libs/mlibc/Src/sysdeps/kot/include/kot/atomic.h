#ifndef KOT_ATOMIC_H
#define KOT_ATOMIC_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

bool atomicLock(uint64_t* mutex, uint64_t bit);
bool atomicUnlock(uint64_t* mutex, uint64_t bit);
void atomicAcquire(uint64_t* mutex, uint64_t bit);

#if defined(__cplusplus)
}
#endif

#endif