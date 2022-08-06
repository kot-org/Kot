#pragma once

#include <kot/heap.h>
#include <kot/utils/map.h>

class SE8LocalVariable {
private:
    vector_t* arr;
public:
    SE8LocalVariable();
    void setInt(uint32_t index, int32_t item);
    int32_t* getInt(uint32_t index);
};