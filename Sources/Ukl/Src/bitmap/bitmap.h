#pragma once
#include <kot/types.h>

class Bitmap{
    public:
        size64_t Size;
        uint8_t* Buffer;
        bool operator[](uint64_t index);
        bool Get(uint64_t index);
        bool Set(uint64_t index, bool value);
        bool GetAndSet(uint64_t index, bool value);
};