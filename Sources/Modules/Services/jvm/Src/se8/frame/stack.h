#pragma once

#include "../types.h"

namespace SE8 {

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

}