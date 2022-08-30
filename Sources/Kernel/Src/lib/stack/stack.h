#pragma once

#include <heap/heap.h>

class KStack {
    private:
        uint64_t top;
        uint64_t sector_size;
        uintptr_t current_sector = NULL;
    public:
        KStack(uint64_t sector_size);
        void sinkInto(uintptr_t dest, uint64_t size);
        uint8_t pop8();
        uint16_t pop16();
        uint32_t pop32();
        uint64_t pop64();
        void push8(uint8_t item);
        void push16(uint16_t item);
        void push32(uint32_t item);
        void push64(uint64_t item);
};
