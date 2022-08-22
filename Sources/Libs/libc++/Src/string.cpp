#include "string.h"

#include <kot/cstring.h>

namespace std {

    StringBuilder::StringBuilder(char* origin) {
        set(origin);
    }

    void StringBuilder::set(char* str) {
        if (buffer != NULL) {
            free(buffer);
        }
        uint64_t fromSize = strlen(str);
        buffer = (char*) malloc(fromSize+1);
        memcpy(buffer, str, fromSize);
        buffer[fromSize] = '\0';
    }

    char* StringBuilder::toString() {
        if (buffer == NULL) {
            return "";
        }
        uint64_t size = strlen(this->buffer);
        char* temp = (char*) malloc(size+1);
        memcpy(temp, buffer, size);
        temp[size] = '\0';
        return temp;
    }

    uint64_t StringBuilder::length() {
        if (buffer == NULL) {
            return 0;
        }
        return strlen(this->buffer);
    }

    void StringBuilder::append(char* str) {
        uint64_t toSize = buffer != NULL ? strlen(this->buffer) : 0;
        uint64_t fromSize = str != NULL ? strlen(str) : 0;
        if (fromSize > 0) {
            if (toSize > 0) {
                uint64_t newSize = toSize+fromSize;
                char* temp = (char*) malloc(newSize+1);
                memcpy(temp, buffer, toSize);
                memcpy((uintptr_t)((uint64_t) temp + toSize), str, fromSize);
                temp[newSize] = '\0';
                free(buffer);
                buffer = temp;
            } else {
                free(this->buffer);
                this->buffer = (char*) malloc(fromSize);
                memcpy(buffer, str, fromSize);
            }
        }
    }

}