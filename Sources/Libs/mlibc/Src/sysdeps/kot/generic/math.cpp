#include <kot/math.h>

extern "C" {

int ipow(int base, int exp){
    int result = 1;
    while(true){
        if (exp & 1){
            result *= base;
        }
        exp >>= 1;
        if (!exp){
            break;
        }
        base *= base;
    }
    return result;
}

int8_t kot_sgn(int64_t x){
    return (x > 0) - (x < 0);
}

}