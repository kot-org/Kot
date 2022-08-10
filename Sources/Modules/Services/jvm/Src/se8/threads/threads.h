#pragma once

#include "thread.h"

namespace SE8 {

    class Threads {
    private:
    public: 
        Threads();
        Thread* create(char* name);
        Thread* get(uint64_t pid);
    };

}