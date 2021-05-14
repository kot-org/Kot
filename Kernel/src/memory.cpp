#include "memory.h"


uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize){

    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        memorySizeBytes += desc->numPages * 4096;
    }

    return memorySizeBytes;

}

void memset(void* start, uint8_t value, uint64_t num){
    for (uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}

__attribute__((optimize("-fno-tree-loop-distribute-patterns"))) void fastMemset(void* start, uint64_t value, uint64_t num){
    if (num <= 8) {
        uint8_t* valPtr = (uint8_t*)&value;
        for (uint8_t* ptr = (uint8_t*)start; ptr < (uint8_t*)((uint64_t)start + num); ptr++) {
            *ptr = *valPtr;
            valPtr++;
        }
        return;
    }
    uint64_t proceedingBytes = num % 8;
    uint64_t newnum = num - proceedingBytes;

    for (uint64_t* ptr = (uint64_t*)start; ptr < (uint64_t*)((uint64_t)start + newnum); ptr++) {
        *ptr = value;
    }
    uint8_t* valPtr = (uint8_t*)&value;
    for (uint8_t* ptr = (uint8_t*)((uint64_t)start + newnum); ptr < (uint8_t*)((uint64_t)start + newnum); ptr++) {
        *ptr = *valPtr;
        valPtr++;
    }
};

void memcpy(void* destination, void* source, uint64_t num){
    if (num <= 8) {
        uint8_t* valPtr = (uint8_t*)&source;
        for (uint8_t* ptr = (uint8_t*)destination; ptr < (uint8_t*)((uint64_t)destination + num); ptr++) {
            *ptr = *valPtr;
            valPtr++;
        }
        return;
    }
    uint64_t proceedingBytes = num % 8;
    uint64_t newnum = num - proceedingBytes;
    uint64_t* srcptr = (uint64_t*)source;

    for (uint64_t* destptr = (uint64_t*)destination; destptr < (uint64_t*)((uint64_t)destination + newnum); destptr++) {
        *destptr = *srcptr;
        srcptr++;
    }
    uint8_t* srcptr8 = (uint8_t*)((uint64_t)source + newnum);
    for (uint8_t* destptr8 = (uint8_t*)((uint64_t)destination + newnum); destptr8 < (uint8_t*)((uint64_t)destination + newnum); destptr8++) {
        *destptr8 = *srcptr8;
        srcptr8++;
    }
}