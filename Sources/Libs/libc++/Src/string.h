#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <kot++/printf.h>

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
        void append(char* str, uint64_t position);
        void append(char c);
        void set(char* str);
        int64_t indexOf(char* str, uint64_t startingAt, bool isLeftStart);
        int64_t indexOf(char* str, uint64_t startingAt);
        int64_t indexOf(char* str);
        uint64_t length();
        ~StringBuilder() {
            free(buffer);
        }
    };

}
