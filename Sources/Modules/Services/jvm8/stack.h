#pragma once

#include <kot/types.h>
#include <kot/heap.h>

class JVM8Stack {
private:
    uintptr_t arr;
    size_t capacity;
    uint64_t top = 0;
public:

    JVM8Stack(size_t capacity);

    bool pushInt(int32_t item);
    int32_t popInt();

};