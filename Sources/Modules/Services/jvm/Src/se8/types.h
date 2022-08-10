#pragma once

#include <kot/types.h>

namespace SE8 {
    
    enum Types {
        Null = 0U,
        NaN = 1U,
        Short = 2U,
        Int = 3U,
        Long = 4U,
        Float = 5U,
        Double = 6U,
        Char = 7U,
        Byte = 8U,
    };

    struct Value {
        uint8_t type;
        uint8_t bytes[];
    };

}