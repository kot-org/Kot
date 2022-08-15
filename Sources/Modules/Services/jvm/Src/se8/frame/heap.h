#pragma once

#include "../types.h"

#include <kot/utils/vector.h>

namespace SE8 {

    class Locals {
    private:
        vector_t* tables;
    public:
        Locals();
        uintptr_t provideTable(uint64_t tableIndex);
        uint64_t getPtr(uint64_t index);
        void setType(uint64_t ptr, uint8_t type);
        uint8_t getType(uint64_t ptr);
        void set32(uint64_t ptr, uint32_t value);
        void set64(uint64_t ptr, uint64_t value);
        uint32_t get32(uint64_t ptr);
        uint64_t get64(uint64_t ptr);
    };

    struct ArrayValue {
        uint8_t bytes[];
    };

    struct Array {
        uint8_t type;
        uint32_t count;
        ArrayValue values[];
    };

}