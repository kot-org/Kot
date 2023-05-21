#pragma once

#include "thread.h"

namespace SE8 {

    class Thread;

    class Threads {
    private:
        kot_vector_t* threads;
    public: 
        Threads();
        Thread* append();
        Thread* Get(uint64_t pid);
    };

}