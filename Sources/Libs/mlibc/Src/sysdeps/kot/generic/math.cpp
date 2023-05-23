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

}