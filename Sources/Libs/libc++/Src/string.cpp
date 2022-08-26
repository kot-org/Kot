#include "string.h"

#include <kot/cstring.h>

namespace std {

    StringBuilder::StringBuilder(char* origin) {
        set(origin);
    }

    int64_t StringBuilder::indexOf(char* str, uint64_t startingAt) {
        uint64_t len = length();
        for (uint64_t i = startingAt; i < len; i++) {
            char* sub = substr(i, len);
            if (strcmp(sub, str)) {
                free(sub);
                return i;
            }
            free(sub);
        }
        return -1;
    }

    int64_t StringBuilder::indexOf(char* str) {
        return indexOf(str, 0);
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

    char* StringBuilder::substr(uint64_t startingAt, uint64_t endingAt) {
        if (buffer == NULL || (endingAt == startingAt) || length() <= startingAt) {
            return "";
        }
        uint64_t size = endingAt - startingAt;
        char* temp = (char*) malloc(size+1);
        memcpy(temp, (uintptr_t)((uint64_t) buffer + startingAt), size);
        temp[size] = '\0';
        return temp;
    }

    char* StringBuilder::toString() {
        return substr(0, length());
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

    void StringBuilder::append(char c) {
        uint64_t toSize = buffer != NULL ? strlen(this->buffer) : 0;
        if (toSize > 0) {
            uint64_t newSize = toSize+1;
            char* temp = (char*) malloc(newSize+1);
            memcpy(temp, buffer, toSize);
            temp[toSize] = c;
            temp[newSize] = '\0';
            free(buffer);
            buffer = temp;
        } else {
            free(this->buffer);
            this->buffer = (char*) malloc(2);
            this->buffer[0] = c;
            this->buffer[1] = '\0';
        }
    }
}