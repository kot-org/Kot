#include "localVariable.h"

// i'll not definitly use a map don't be afraid

namespace SE8 {

    LocalVariable::LocalVariable() {
        this->arr = map_create();
    }

    void LocalVariable::setInt(uint32_t index, int32_t item) {
        map_set(arr, index, &item);
    }

    int32_t* LocalVariable::getInt(uint32_t index) {
        return (int32_t*) map_get(arr, index);
    }

    void LocalVariable::setLong(uint32_t index, int64_t item) {
        map_set(arr, index, &item);
    }

    int64_t* LocalVariable::getLong(uint32_t index) {
        return (int64_t*) map_get(arr, index);
    }

    void LocalVariable::setFloat(uint32_t index, float item) {
        map_set(arr, index, &item);
    }

    float* LocalVariable::getFloat(uint32_t index) {
        return (float*) map_get(arr, index);
    }

    void LocalVariable::setDouble(uint32_t index, double item) {
        map_set(arr, index, &item);
    }

    double* LocalVariable::getDouble(uint32_t index) {
        return (double*) map_get(arr, index);
    }

}