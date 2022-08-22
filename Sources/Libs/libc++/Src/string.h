#pragma once

#include <kot/heap.h>

namespace std {
    
    class StringBuilder {
    private:
        char* buffer = NULL;
    public:
        StringBuilder() {}
        StringBuilder(char* origin);
        char* toString();
        void append(char* str);
        void set(char* str);
        uint64_t length();
        ~StringBuilder() {
            free(buffer);
        }
    };

}
