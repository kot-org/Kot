#pragma once

#include "thread.h"

namespace SE8 {

    class Thread;

    class Threads {
    private:
        vector_t* threads;
    public: 
        Threads();
        Thread* append();
        Thread* get(uint64_t pid);
    };

}