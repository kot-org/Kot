#include "threads.h"

namespace SE8 {

    Threads::Threads() {
        threads = vector_create();
    }

    Thread* Threads::append() {
        for (uint64_t i = 0; i < threads->length; i++) {
            Thread* thr = (Thread*) vector_get(threads, i);
            if (!thr->isUsed()) {
                return thr;
            }
        }
        Thread* thr = new Thread(threads->length+1);
        vector_push(threads, thr);
        return thr;
    }

    Thread* Threads::Get(uint64_t pid) {
        if (pid <= threads->length) {
            return (Thread*) vector_get(threads, pid);
        }
        return NULL;
    }

} 
