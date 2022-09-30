#include <lib/math.h>

uint32_t DivideRoundUp(uint32_t value, uint32_t divider){
    uint32_t returnValue = value / divider;
    if((value % divider) != 0){
        returnValue++;
    }
    return returnValue;
}