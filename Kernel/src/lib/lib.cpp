#include "lib.h"


bool ReadBit(uint8_t byte, int position)
{
    return (byte >> position) & 0x1;
}

uint8_t WriteBit(uint8_t byte, int position, bool value)
{
    if(value){
        byte |= 1 << position;
    }else{
        byte &= ~(1 << position);
    }

    return byte;    
}


uint64_t Divide(uint64_t value, uint64_t divider){
    uint64_t returnValue = value / divider;
    if((value % divider) != 0){
        returnValue++;
    }
    return returnValue;
}

bool CompareGUID(GUID* first, GUID* second){
    if(first->Data1 == second->Data1 && first->Data2 == second->Data2 && first->Data3 == second->Data3 && first->Data4 == second->Data4) return true;
    return false;
}