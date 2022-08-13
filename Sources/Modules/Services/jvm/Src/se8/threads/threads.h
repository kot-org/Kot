#pragma once

#include "thread.h"

namespace SE8 {

    class Threads {
    private:
    public: 
        Threads();
        thread* create(char* name);
        thread* get(uint64_t pid);
    };

}