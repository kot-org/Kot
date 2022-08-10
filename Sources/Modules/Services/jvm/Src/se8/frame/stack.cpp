#include "stack.h"

uint8_t getTypeSize(uint8_t type) {
    switch (type) {
        case SE8::Null:
        case SE8::NaN:
            return 0;
        case SE8::Byte:
            return 1;
        case SE8::Char:
        case SE8::Short:
            return 2;
        case SE8::Int:
        case SE8::Float:
            return 4;
        case SE8::Long:
        case SE8::Double:
            return 8;
    }
}

namespace SE8 {

    Stack::Stack(size_t capacity) {
        this->capacity = capacity;
        this->arr = malloc(this->capacity);
    }

    void Stack::push(Value* value) {
        uint8_t size = getTypeSize(value->type);
        if (size + 1 > capacity) {
            // stack overflow
            return;
        }
        if (size != 0) { memcpy((uintptr_t)((uint64_t) arr + top), (uintptr_t)((uint64_t) value + 1), size); top += size; }
        *(uint8_t*)((uint64_t) arr + top) = value->type;
        top++;
    }

    Value* Stack::pop() {
        top--;
        uint8_t type = *(uint8_t*)((uint64_t) arr + top);
        uint8_t size = getTypeSize(type);
        if (size == 0) {
            Value* ret = (Value*) malloc(1);
            ret->type = type;
            return ret;
        } else {
            Value* ret = (Value*) malloc(size + 1);
            ret->type = type;
            top-=size;
            memcpy((uintptr_t)((uint64_t) ret + 1), (uintptr_t)((uint64_t) arr + top), size); 
            return ret;
        }
    }

    Value* Stack::peek() {
        uint8_t type = *(uint8_t*)((uint64_t) arr + (top - 1));
        uint8_t size = getTypeSize(type);
        if (size == 0) {
            Value* ret = (Value*) malloc(1);
            ret->type = type;
            return ret;
        } else {
            Value* ret = (Value*) malloc(size + 1);
            ret->type = type;
            memcpy((uintptr_t)((uint64_t) ret + 1), (uintptr_t)((uint64_t) arr + (top - size - 1)), size); 
            return ret;
        }
    }

    void Stack::pushNull() {
        if (top + 1 > capacity) {
            // stack overflow
            return;
        }
        *(uint8_t*)((uint64_t) arr + top) = SE8::Null;
        top++;
    }

    void Stack::pushNaN() {
        if (top + 1 > capacity) {
            // stack overflow
            return;
        }
        *(uint8_t*)((uint64_t) arr + top) = SE8::NaN;
        top++;
    }

    void Stack::pushByte(uint8_t item) {
        if (top + 2 > capacity) {
            // stack overflow
            return;
        }
        *(uint8_t*)((uint64_t) arr + top) = item;
        top++;
        *(uint8_t*)((uint64_t) arr + top) = SE8::Byte;
        top++;
    }

    void Stack::pushChar(uint16_t item) {
        if (top + 3 > capacity) {
            // stack overflow
            return;
        }
        *(uint16_t*)((uint64_t) arr + top) = item;
        top+=2;
        *(uint8_t*)((uint64_t) arr + top) = SE8::Char;
        top++;
    }

    void Stack::pushShort(int16_t item) {
        if (top + 3 > capacity) {
            // stack overflow
            return;
        }
        *(int16_t*)((uint64_t) arr + top) = item;
        top+=2;
        *(uint8_t*)((uint64_t) arr + top) = SE8::Short;
        top++;
    }

    void Stack::pushInt(int32_t item) {
        if (top + 5 > capacity) {
            // stack overflow
            return;
        }
        *(int32_t*)((uint64_t) arr + top) = item;
        top+=4;
        *(uint8_t*)((uint64_t) arr + top) = SE8::Int;
        top++;
    }
    
    void Stack::pushLong(int64_t item) {
        if (top + 9 > capacity) {
            // stack overflow
            return;
        }
        *(int64_t*)((uint64_t) arr + top) = item;
        top+=8;
        *(uint8_t*)((uint64_t) arr + top) = SE8::Long;
        top++;
    }

    void Stack::pushFloat(float item) {
        if (top + 5 > capacity) {
            // stack overflow
            return;
        }
        *(float*)((uint64_t) arr + top) = item;
        top+=4;
        *(uint8_t*)((uint64_t) arr + top) = SE8::Float;
        top++;
    }

    void Stack::pushDouble(double item) {
        if (top + 9 > capacity) {
            // stack overflow
            return;
        }
        *(double*)((uint64_t) arr + top) = item;
        top+=8;
        *(uint8_t*)((uint64_t) arr + top) = SE8::Double;
        top++;
    }

}
