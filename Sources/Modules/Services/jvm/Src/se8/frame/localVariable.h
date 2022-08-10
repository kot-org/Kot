#pragma once

#include <kot/heap.h>
#include <kot/utils/map.h>

namespace SE8 {

    class LocalVariable {
    private:
        vector_t* arr;
    public:
        LocalVariable();
        void setInt(uint32_t index, int32_t item);
        int32_t* getInt(uint32_t index);
        void setLong(uint32_t index, int64_t item);
        int64_t* getLong(uint32_t index);
        void setFloat(uint32_t index, float item);
        float* getFloat(uint32_t index);
        void setDouble(uint32_t index, double item);
        double* getDouble(uint32_t index);
    };

}