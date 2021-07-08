#include "lib.h"


bool ReadBit(uint8_t byte, int position)
{
    return (byte >> position) & 0x1;
}

void WriteBit(uint8_t byte, int position, bool value)
{
    if(value){
        byte |= 1 << position;
    }else{
        byte &= ~(1 << position);
    }    
}