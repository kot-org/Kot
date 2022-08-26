#pragma once

#include <kot/heap.h>

namespace std {
    
    class StringBuilder {
    private:
        char* buffer = NULL;
    public:
        StringBuilder() {}
        StringBuilder(char* origin);
        char* substr(uint64_t startingAt, uint64_t endingAt);
        char* toString();
        void append(char* str);
        void append(char c);
        void set(char* str);
        int64_t indexOf(char* str, uint64_t startingAt);
        int64_t indexOf(char* str);
        uint64_t length();
        ~StringBuilder() {
            free(buffer);
        }
    };

}
