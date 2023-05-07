#include "__cxxabi_config.h"

#include <assert.h>
#include <stdint.h>
#include <__threading_support>

/*
    This implementation must be careful to not call code external to this file
    which will turn around and try to call __cxa_guard_acquire reentrantly.
    For this reason, the headers of this file are as restricted as possible.

    In addition, when using the framework OS, we cannot actually assume that mutex
    and semaphore are safe for use. For example, with FreeRTOS, static initialization
    can happen before the scheduler is set up. We are hosed in that case.
*/

namespace __cxxabiv1
{

namespace
{

#if defined(__LP64__)

typedef uint64_t guard_type;
#define CompareAndSwap(ptr, old, new_val) __sync_bool_compare_and_swap_8(ptr, old, new_val)
#define AtomicOr(ptr, mask) __sync_or_and_fetch_8(ptr, mask)

#else //32-bit
// A 32-bit, 4-byte-aligned static data value. The least significant 2 bits must
// be statically initialized to 0.
typedef uint32_t guard_type;
#define CompareAndSwap(ptr, old, new_val) __sync_bool_compare_and_swap_4((volatile int*)ptr, old, new_val)
#define AtomicOr(ptr, mask) __sync_or_and_fetch_4(ptr, mask)

#endif

#define INITIALIZED_FLAG (1U << 0)
#define INITIALIZER_IN_USE (1U << 1)
#define IM_THE_SUPERVISOR (1U << 2)

inline void set_initialized(volatile guard_type* guard_object)
{
    AtomicOr(guard_object, INITIALIZED_FLAG);
}

// Test the lowest bit.
inline bool is_initialized(volatile guard_type* guard_object)
{
    guard_type val = *guard_object;
    return val & INITIALIZED_FLAG;
}

inline void set_initializer_in_use(volatile guard_type* guard_object)
{
    AtomicOr(guard_object, INITIALIZER_IN_USE);
}

inline bool initializer_in_use(volatile guard_type* guard_object)
{
    guard_type val = *guard_object;
    return val & INITIALIZER_IN_USE;
}

}  // unnamed namespace

extern "C"
{

#ifndef _LIBCXXABI_HAS_NO_THREADS
_LIBCXXABI_FUNC_VIS int __cxa_guard_acquire(guard_type *guard_object)
{
    volatile guard_type *p_obj = guard_object;

    if(is_initialized(p_obj))
    {
        // We've already run the guard
        return 0;
    }

    guard_type old_val = *p_obj;

    /*
     * Remove the "i'm the supervisor" and "in use" flags for the compare-and-swap.
     * We only want this happening once. So, basically, if another thread is
     * currently doing this already, we want the CAS to fail.  CAS with old and
     * new value the same may just work. So we head that failure case off at
     * the pass, and force the second thread to try to enter this section to
     * block until the initializer is done.
     *
     * In most cases, this will boil down to a NOP.
     */
    old_val &= ~(INITIALIZED_FLAG | IM_THE_SUPERVISOR);

    // Only try once, which is why we don't use atomic_or().
    if(CompareAndSwap(p_obj, old_val, (old_val | IM_THE_SUPERVISOR)))
    {
        // We won the race to run the initializer, so tell the caller to do it

        if(initializer_in_use(p_obj))
        {
            // This condition should not happen - it means CAS failed us and two threads made it here.
            assert(0);
            while(1);
        }

        // Also, set this guard variable as in use
        set_initializer_in_use(p_obj);

        return 1;
    }
    else
    {
        /*
         * We lost the race, block until initer has finished running.
         *
         * NOTE: If the same thread gets back into the same initializer function during
         * the initializer process, it will hang indefinitely. This is okay, per
         * the spec, however, an explicit panic() would be better for debug.
         *
         * Section 6.7 of the C++ Spec says "If control re-enters the declaration
         * recursively while the object is being initialized, the behavior is
         * undefined".
         */
        while(!is_initialized(p_obj))
        {
            std::__libcpp_thread_yield();
        }
    }

    /*
     * At this point, the initializer is done, so we can resume this stalled
     * thread. However, we should tell the caller not to run the initializer.
     */
    return 0;
}

_LIBCXXABI_FUNC_VIS void __cxa_guard_release(guard_type *guard_object)
{
    volatile guard_type* p_obj = guard_object;
    set_initialized(p_obj);
}

_LIBCXXABI_FUNC_VIS void __cxa_guard_abort(guard_type *guard_object)
{
    volatile guard_type *p_obj = guard_object;
    guard_type old_val, new_val;

    do {
        // Clear both the in-use flag, and the "i'm the supervisor" flag
        old_val = *p_obj;
        new_val = old_val & ~(INITIALIZER_IN_USE | IM_THE_SUPERVISOR);
    } while(CompareAndSwap(p_obj, old_val, new_val));
}

#else // _LIBCXXABI_HAS_NO_THREADS

_LIBCXXABI_FUNC_VIS int __cxa_guard_acquire(guard_type *guard_object) {
    return !is_initialized((volatile guard_type*)guard_object);
}

_LIBCXXABI_FUNC_VIS void __cxa_guard_release(guard_type *guard_object) {
    *guard_object = 0;
    set_initialized((volatile guard_type*)guard_object);
}

_LIBCXXABI_FUNC_VIS void __cxa_guard_abort(guard_type *guard_object) {
    *guard_object = 0;
}

#endif // !_LIBCXXABI_HAS_NO_THREADS

}  // extern "C"

}  // __cxxabiv1
