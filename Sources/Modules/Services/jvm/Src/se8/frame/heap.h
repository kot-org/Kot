#pragma once

#include "../types.h"

#include <kot/utils/vector.h>

// todo: remake Locals

namespace SE8 {

    class Locals {
    private:
        vector_t* tables;
        uint64_t tableSize = 300;
    public:
        Locals(uint64_t maxLocals);
        uintptr_t provideTable(uint64_t tableIndex);
        uint64_t getPtr(uint64_t index);
        void setType(uint64_t ptr, uint8_t type);
        uint8_t getType(uint64_t ptr);
        void set32(uint64_t ptr, uint32_t value);
        void set64(uint64_t ptr, uint64_t value);
        uint32_t get32(uint64_t ptr);
        uint64_t get64(uint64_t ptr);
    };

    class Stack {
    private:
        uintptr_t arr;
        size_t capacity;
        uint64_t top = 0;
    public:
        
        Stack(size_t capacity);

        void push(Value* value);

        Value* pop();
        Value* peek();
        void wpop(); // pop without getting the value
        
        void empty();

        void pushNull();
        void pushNaN();
        void pushByte(uint8_t item);
        void pushChar(uint16_t item);
        void pushShort(int16_t item);
        void pushInt(int32_t item);
        void pushLong(int64_t item);
        void pushFloat(float item);
        void pushDouble(double item);
        void pushArrayRef(uint64_t pointer);
        
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