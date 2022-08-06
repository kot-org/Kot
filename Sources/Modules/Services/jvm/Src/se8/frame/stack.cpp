#include "stack.h"

SE8Stack::SE8Stack(size_t capacity) {
    this->capacity = capacity;
    this->arr = malloc(this->capacity);
}

/**
 * @return false StackOverflow
 */
bool SE8Stack::pushInt(int32_t item) {
    if (top + sizeof(int32_t) > capacity) {
        return false;
    }
    *(int32_t*)((uint64_t) arr + top) = item;
    top += sizeof(int32_t);
    return true;
}

/**
 * @return NULL StackLowflow
 */
int32_t SE8Stack::popInt() {
    if (top - sizeof(int32_t) < 0) {
        return NULL;
    }
    top -= sizeof(int32_t);
    return *(int32_t*)((uint64_t) arr + top);
}