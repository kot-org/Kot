#include <utils/mmio.h>

void MMIOWrite32(uintptr_t addr, uint64_t reg, uint32_t value){
    *(volatile uint32_t*)((uint64_t)addr + reg) = value;
}

uint8_t MMIORead8(uintptr_t addr, uint64_t reg){
    return *(volatile uint8_t*)((uint64_t)addr + reg);
}

uint32_t MMIORead32(uintptr_t addr, uint64_t reg){
    return *(volatile uint32_t*)((uint64_t)addr + reg);
}