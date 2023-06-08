#pragma once

#include <stdlib.h>

struct Reader {

    uintptr_t buffer = NULL;
    uint64_t index = 0;

    uint8_t u1() {
        index++;
        return *(uint8_t*)((uint64_t) buffer + (index - 1));
    }

    uint16_t u2L() { // little endian
        index+=2;
        return *(uint16_t*)((uint64_t) buffer + (index - 2));
    }  

    uint32_t u4L() { // little endian
        index+=4;
        return *(uint32_t*)((uint64_t) buffer + (index - 4));
    }  

    uint64_t u8L() { // little endian
        index+=8;
        return *(uint64_t*)((uint64_t) buffer + (index - 8));
    }  

    uint16_t u2B() { // big endian
        return (u1() << 8) | u1();
    }   

    uint32_t u4B() { // big endian
        return (u2B() << 16) | u2B();
    }

    uint64_t u8B() { // big endian
        return (u4B() << 32) | u4B();
    }

    uint8_t* uL(uint64_t length) {
        if (length == 0) return NULL;
        uint8_t* dest = (uint8_t*) malloc(length);
        memcpy(dest, (uintptr_t)((uint64_t) buffer + index), length);
        index += length;
        return dest;
    }

    uint8_t* uB(uint64_t length) {
        if (length == 0) return NULL;
        uint8_t* dest = (uint8_t*) malloc(length);
        index += length;
        for (uint64_t i = 0; i < length; i++) {
            index--;
            dest[i] = *(uint8_t*)((uint64_t) buffer + index);
        }
        index += length;
        return dest;
    }

};
