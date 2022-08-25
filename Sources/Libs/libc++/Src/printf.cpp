#include "printf.h"
#include "string.h"

namespace std {

    void printf(const char* str, ...) {
        __builtin_va_list args;
        __builtin_va_start(args, str);

        char c;
        StringBuilder* strBuilder = new StringBuilder();

        while((c = *str++) != 0) {
            if(c == '%') {
                c = *str++;
                
                switch (c)
                {
                    case 'x':

                        break;
                    
                    default:
                        break;
                }
            } else {
                strBuilder->append(&c);
            }
        }
        Printlog(strBuilder->toString());

        __builtin_va_end(args);
    }

}