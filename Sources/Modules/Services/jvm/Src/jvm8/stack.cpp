#include <jvm8/stack.h>
#include <kot/cstring.h>

JVM8Stack::JVM8Stack(size_t capacity) {
    this->capacity = capacity;
    this->arr = (uintptr_t*) malloc(this->capacity);
    this->top = capacity/sizeof(uintptr_t);
    char buffer[255];
    Printlog(itoa(top, buffer, 10));
}

bool JVM8Stack::isFull() {
    if (index==top) {
        return true;
    }
    return false;
}

/**
 * @return false Stackoverflow
 */
bool JVM8Stack::push(uintptr_t item) {
    if (isFull()) {
        return false;
    } else {
        this->arr[top++] = item;
        return true;
    }
}

uintptr_t JVM8Stack::pop() {
    if (this->index == -1) {
        return NULL;
    } else {
        uintptr_t temp = this->arr[index];
        this->arr[index] = NULL;
        this->index--;
        return temp;
    }
}

uintptr_t JVM8Stack::peek() {
    if (this->index == -1) {
        return NULL;
    } else {
        return this->arr[index];
    }
}