#include <utils/mmio.h>

void MMIOWrite32(uint64_t addr, uint32_t value) {
    *(volatile uint32_t*) addr = value;
}

uint32_t MMIORead32(uint64_t addr) {
    return *(volatile uint32_t*) addr;
}