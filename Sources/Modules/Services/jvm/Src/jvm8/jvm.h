#pragma once

#include <jvm8/stack.h>
#include <jvm8/heap.h>
#include <jvm8/methodArea.h>
#include <jvm8/opcodes.h>

class JVM8 {
private:
    JVM8Stack* stack;
    JVM8Heap* heap;
    JVM8MethodArea* methodArea;
    JVM8OpCodesTable* opCodesTable;
    uint64_t pc = 0;
public:
    JVM8(size_t stackCapacity);
    void initialize();
};