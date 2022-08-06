#pragma once

#include <kot/heap.h>
#include <kot/utils/map.h>

class JVM8LocalVariable {
private:
    vector_t* arr;
public:
    JVM8LocalVariable();
    void setInt(uint32_t index, int32_t item);
    int32_t* getInt(uint32_t index);
};