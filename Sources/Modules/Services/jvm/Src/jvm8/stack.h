#pragma once

#include <kot/types.h>
#include <kot/heap.h>

class JVM8Stack {
private:
    uintptr_t* arr;
    size_t capacity;
    int64_t index = -1;
    int64_t top;
public:
    JVM8Stack(size_t capacity);
    bool isFull();
    bool push(uintptr_t item);
    uintptr_t pop();
    uintptr_t peek();
};