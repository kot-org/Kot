#include <kot/math.h>

uint64_t DivideRoundUp(uint64_t value, uint64_t divider){
    uint64_t returnValue = value / divider;
    if((value % divider) != 0){
        returnValue++;
    }
    return returnValue;
}

int8_t sgn(int64_t value) {
    if (value < 0) return -1;
    if (value > 0) return 1;
    return 0;
}

int64_t abs(int64_t value) {
    return value * ((value > 0) - (value < 0));
}

int64_t min(int64_t a, int64_t b) {
    return (a > b) ? b : a;
}

int64_t max(int64_t a, int64_t b) {
    return (a > b) ? a : b;
}

uint64_t divideRoundUp(uint64_t value, uint64_t divider) {
    uint64_t returnValue = value / divider;
    if((value % divider) != 0){
        returnValue++;
    }
    return returnValue;
}