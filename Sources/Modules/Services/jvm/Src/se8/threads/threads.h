#pragma once

#include "thread.h"

namespace SE8 {

    class threads {
    private:
    public: 
        threads();
        thread* create(char* name);
        thread* get(uint64_t pid);
    };

}