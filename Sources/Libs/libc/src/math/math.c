#include <kot/math.h>

uint64_t DivideRoundUp(uint64_t value, uint64_t divider){
    uint64_t returnValue = value / divider;
    if((value % divider) != 0){
        returnValue++;
    }
    return returnValue;
}