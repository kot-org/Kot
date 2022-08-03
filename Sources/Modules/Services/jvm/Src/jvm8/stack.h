#pragma once

#include <kot/types.h>
#include <kot/heap.h>

class JVM8Stack {
private:
    uintptr_t* arr;
    size_t capacity;
    int64_t top = -1;
    int64_t maxTop;
public:
    JVM8Stack(size_t capacity);
    bool isFull();
    bool isEmpty();
    bool push(uintptr_t item);
    uintptr_t pop();
    uintptr_t peek();
};