#include <jvm8/localVariable.h>

// i'll not use a map don't be afraid

JVM8LocalVariable::JVM8LocalVariable() {
    this->arr = map_create();
}

void JVM8LocalVariable::setInt(uint32_t index, int32_t item) {
    map_set(arr, (char*) index, &item);
}

int32_t* JVM8LocalVariable::getInt(uint32_t index) {
    return (int32_t*) map_get(arr, (char*) index);
}