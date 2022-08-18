#include <tools/config.h>

void PCIMemcpyToMemory8(uintptr_t dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x1){
        *(uint8_t*)((uint64_t)dst + i) = PCIRead8(src + i);
    }
}

void PCIMemcpyToMemory16(uintptr_t dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x2){
        *(uint16_t*)((uint64_t)dst + i) = PCIRead16(src + i);
    }
}

void PCIMemcpyToMemory32(uintptr_t dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x4){
        *(uint32_t*)((uint64_t)dst + i) = PCIRead32(src + i);
    }
}