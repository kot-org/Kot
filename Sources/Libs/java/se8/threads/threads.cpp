#include "threads.h"

namespace SE8 {

    Threads::Threads() {
        threads = kot_vector_create();
    }

    Thread* Threads::append() {
        for (uint64_t i = 0; i < threads->length; i++) {
            Thread* thr = (Thread*) kot_vector_get(threads, i);
            if (!thr->isUsed()) {
                return thr;
            }
        }
        Thread* thr = new Thread(threads->length+1);
        kot_vector_push(threads, thr);
        return thr;
    }

    Thread* Threads::Get(uint64_t pid) {
        if (pid <= threads->length) {
            return (Thread*) kot_vector_get(threads, pid);
        }
        return NULL;
    }

} 
