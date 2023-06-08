#include "printf.h"
#include <stdio.h>
#include <stdarg.h>

namespace std {

    void printf(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }

}
