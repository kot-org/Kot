#include <jvm8/stack.h>
#include <kot/cstring.h>

JVM8Stack::JVM8Stack(size_t capacity) {
    this->capacity = capacity;
    this->arr = (uintptr_t*) malloc(this->capacity);
    this->maxTop = capacity/sizeof(uintptr_t)-1;
}

bool JVM8Stack::isFull() {
    if (top==maxTop) {
        return true;
    }
    return false;
}

bool JVM8Stack::isEmpty() {
    return this->top == -1;
}


/**
 * @return false Stackoverflow
 */
bool JVM8Stack::push(uintptr_t item) {
    if (isFull()) {
        return false;
    } else {
        top++;
        *(this->arr + top) = item;
        return true;
    }
}

uintptr_t JVM8Stack::pop() {
    if (this->top == -1) {
        return NULL;
    } else {
        uintptr_t temp = *(this->arr + top);
        *(this->arr + top) = NULL;
        this->top--;
        return temp;
    }
}

uintptr_t JVM8Stack::peek() {
    if (this->top == -1) {
        return NULL;
    } else {
        return *(this->arr + top);
    }
}