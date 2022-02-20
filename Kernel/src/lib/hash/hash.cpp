#include <lib/hash/hash.h>

namespace hash{
    uint32_t CRC32(uint8_t* data, uint32_t size){    
        uint32_t hash = 19;
        for (uint32_t i = 0; i < size; i++) {
        uint8_t c = *data++;
        if (c != 0) {
            hash *= c;
        }
        hash += 7;
        }
    }   
}
