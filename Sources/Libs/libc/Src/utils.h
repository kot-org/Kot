#ifndef _UTILS_H
#define _UTILS_H 1

#define Kot_VendorID 0x107111116

static inline bool IsBeetween(uint64_t lower, uint64_t value, uint64_t upper){
    return (lower <= value) && (upper >= value);
}

static inline bool ReadBit(uint8_t byte, int position){
    return (byte >> position) & 0x1;
}

static inline uint8_t WriteBit(uint8_t byte, int position, bool value){
    if(value){
        byte |= 1 << position;
    }else{
        byte &= ~(1 << position);
    }

    return byte;    
}


#endif