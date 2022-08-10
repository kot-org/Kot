#pragma once

#include "loader.h"

#include <kot/utils/map.h>

namespace SE8 {

    class Classes {
    private:
        vector_t* areas;
    public:
        Classes();
        void loadClassBytes(uintptr_t bytes);
    };

}