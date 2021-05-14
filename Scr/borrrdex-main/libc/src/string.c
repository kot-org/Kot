#include "string.h"
#include "stdint.h"

void* memcpy(void* __restrict dest, const void* __restrict src, size_t n) {
    if(!((uintptr_t)dest & (sizeof(intptr_t) - 1)) && !((uintptr_t)src & sizeof(intptr_t) - 1)) {
        uintptr_t* d = (uintptr_t *)dest;
        uintptr_t* s = (uintptr_t *)src;
        while(n >= sizeof(intptr_t)) {
            *d++ = *s++;
            n -= sizeof(intptr_t);
        }

        dest = d;
        src = s;
    }

    uint8_t* d = (uint8_t *)dest;
    uint8_t* s = (uint8_t *)src;
    while(n--) {
        *d++ = *s++;
    }
}

__attribute__((pure)) int memcmp(const void* aptr, const void* bptr, size_t n) {
    uint8_t* a = (uint8_t *)aptr;
    uint8_t* b = (uint8_t *)bptr;
    for(size_t i = 0; i < n; i++) {
        if(*a != *b) {
            return *a - *b;
        }

        a++;
        b++;
    }

    return 0;
}

void memset(void* dst, int c, size_t n) {
    uint8_t* current = (uint8_t *)dst;
    for(uint64_t i = 0; i < n; i++) {
        *current++ = c;
    }
}