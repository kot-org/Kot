#include "stack.h"

namespace std {

    Stack::Stack(uint64_t sector_size) {
        if (sector_size < 24) sector_size = 24;
        this->sector_size = sector_size + 8 + 1; // 8: last table address; 1: recovery byte
        current_sector = malloc(sector_size);
        *(uint64_t*)(current_sector) = (uint64_t) NULL;
        top = 8;
    }

    uint8_t Stack::pop8() {
        if (top - 1 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_sector);
            if (temp == NULL) {
                return NULL;
            }
            free(current_sector);
            current_sector = temp;
            top = sector_size - *(uint8_t*)((uint64_t) current_sector + sector_size - 1);
            return pop8();
        } else {
            top--;
            return *(uint8_t*)((uint64_t) current_sector + top);
        }
    }

    uint16_t Stack::pop16() {
        if (top - 2 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_sector);
            if (temp == NULL) {
                return NULL;
            }
            free(current_sector);
            current_sector = temp;
            top = sector_size - *(uint8_t*)((uint64_t) current_sector + sector_size - 1);
            return pop16();
        } else {
            top-=2;
            return *(uint16_t*)((uint64_t) current_sector + top);
        }
    }

    uint32_t Stack::pop32() {
        if (top - 4 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_sector);
            if (temp == NULL) {
                return NULL;
            }
            free(current_sector);
            current_sector = temp;
            top = sector_size - *(uint8_t*)((uint64_t) current_sector + sector_size - 1);
            return pop32();
        } else {
            top-=4;
            return *(uint32_t*)((uint64_t) current_sector + top);
        }
    }

    uint64_t Stack::pop64() {
        if (top - 8 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_sector);
            if (temp == NULL) {
                return NULL;
            }
            free(current_sector);
            current_sector = temp;
            top = sector_size - *(uint8_t*)((uint64_t) current_sector + sector_size - 1);
            return pop64();
        } else {
            top-=8;
            std::printf("%x %x %x", (uint64_t)current_sector + top, current_sector, top);
            return *(uint64_t*)((uint64_t) current_sector + top);
        }
    }

    uint8_t Stack::peek8() {
        if (top - 1 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_sector);
            if (temp == NULL) {
                return NULL;
            }
            uint64_t temp_top = sector_size - *(uint8_t*)((uint64_t) current_sector + sector_size - 1);
            return *(uint8_t*)((uint64_t) temp + temp_top - 1);
        } else {
            return *(uint8_t*)((uint64_t) current_sector + top - 1);
        }
    }

    uint16_t Stack::peek16() {
        if (top - 2 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_sector);
            if (temp == NULL) {
                return NULL;
            }
            uint64_t temp_top = sector_size - *(uint8_t*)((uint64_t) current_sector + sector_size - 1);
            return *(uint16_t*)((uint64_t) temp + temp_top - 2);
        } else {
            return *(uint16_t*)((uint64_t) current_sector + top - 2);
        }
    }

    uint32_t Stack::peek32() {
        if (top - 4 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_sector);
            if (temp == NULL) {
                return NULL;
            }
            uint64_t temp_top = sector_size - *(uint8_t*)((uint64_t) current_sector + sector_size - 1);
            return *(uint32_t*)((uint64_t) temp + temp_top - 4);
        } else {
            return *(uint32_t*)((uint64_t) current_sector + top - 4);
        }
    }

    uint64_t Stack::peek64() {
        if (top - 8 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_sector);
            if (temp == NULL) {
                return NULL;
            }
            uint64_t temp_top = sector_size - *(uint8_t*)((uint64_t) current_sector + sector_size - 1);
            return *(uint64_t*)((uint64_t) temp + temp_top - 8);
        } else {
            return *(uint64_t*)((uint64_t) current_sector + top - 8);
        }
    }

    void Stack::push8(uint8_t item) {
        if (top + 1 > sector_size - 1) {
            uintptr_t temp = malloc(sector_size);
            *(uint64_t*)(temp) = (uint64_t) current_sector;
            current_sector = temp;
            top = 8;
            push8(item);
        } else {
            *(uint8_t*)((uint64_t) current_sector + top) = item;
            top++;
        }
    }

    void Stack::push16(uint16_t item) {
        if (top + 2 > sector_size - 1) {
            *(uint8_t*)((uint64_t) current_sector + sector_size - 1) = sector_size - top;
            uintptr_t temp = malloc(sector_size);
            *(uint64_t*)(temp) = (uint64_t) current_sector;
            current_sector = temp;
            top = 8;
            push16(item);
        } else {
            *(uint16_t*)((uint64_t) current_sector + top) = item;
            top += 2;
        }
    }

    void Stack::push32(uint32_t item) {
        if (top + 4 > sector_size - 1) {
            *(uint8_t*)((uint64_t) current_sector + sector_size - 1) = sector_size - top;
            uintptr_t temp = malloc(sector_size);
            *(uint64_t*)(temp) = (uint64_t) current_sector;
            current_sector = temp;
            top = 8;
            push32(item);
        } else {
            *(uint32_t*)((uint64_t) current_sector + top) = item;
            top += 4;
        }
    }

    void Stack::push64(uint64_t item) {
        if (top + 8 > sector_size - 1) {
            *(uint8_t*)((uint64_t) current_sector + sector_size - 1) = sector_size - top;
            uintptr_t temp = malloc(sector_size);
            *(uint64_t*)(temp) = (uint64_t) current_sector;
            current_sector = temp;
            top = 8;
            push64(item);
        } else {
            std::printf("%x", (uint64_t)current_sector + top);
            *(uint64_t*)((uint64_t) current_sector + top) = item;
            top += 8;
        }
    }

    void Stack::sinkInto(uintptr_t dest, uint64_t size) {
        uint64_t idx = (uint64_t) dest + size;
        while (idx != (uint64_t) dest) {
            idx--;
            *(uint8_t*)(idx) = pop8();
        }
    }

}
