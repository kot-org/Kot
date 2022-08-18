#include "stack.h"

namespace std {

    Stack::Stack(uint64_t table_size) {
        if (table_size < 24) table_size = 24;
        this->table_size = table_size + 8 + 1; // 8: last table address; 1: recovery byte
        current_table = malloc(table_size);
        *(uint64_t*)(current_table) = (uint64_t) NULL;
        top = 8;
    }

    uint8_t Stack::pop8() {
        if (top - 1 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_table);
            if (temp == NULL) {
                return NULL;
            }
            free(current_table);
            current_table = temp;
            top = table_size - *(uint8_t*)((uint64_t) current_table + table_size - 1);
            return pop8();
        } else {
            top--;
            return *(uint8_t*)((uint64_t) current_table + top);
        }
    }

    uint16_t Stack::pop16() {
        if (top - 2 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_table);
            if (temp == NULL) {
                return NULL;
            }
            free(current_table);
            current_table = temp;
            top = table_size - *(uint8_t*)((uint64_t) current_table + table_size - 1);
            return pop16();
        } else {
            top-=2;
            return *(uint16_t*)((uint64_t) current_table + top);
        }
    }

    uint32_t Stack::pop32() {
        if (top - 4 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_table);
            if (temp == NULL) {
                return NULL;
            }
            free(current_table);
            current_table = temp;
            top = table_size - *(uint8_t*)((uint64_t) current_table + table_size - 1);
            return pop32();
        } else {
            top-=4;
            return *(uint32_t*)((uint64_t) current_table + top);
        }
    }

    uint64_t Stack::pop64() {
        if (top - 8 < 8) {
            uintptr_t temp = (uintptr_t) *(uint64_t*)(current_table);
            if (temp == NULL) {
                return NULL;
            }
            free(current_table);
            current_table = temp;
            top = table_size - *(uint8_t*)((uint64_t) current_table + table_size - 1);
            return pop64();
        } else {
            top-=8;
            return *(uint64_t*)((uint64_t) current_table + top);
        }
    }

    void Stack::push8(uint8_t item) {
        if (top + 1 > table_size - 1) {
            uintptr_t temp = malloc(table_size);
            *(uint64_t*)(temp) = (uint64_t) current_table;
            current_table = temp;
            top = 8;
            push8(item);
        } else {
            *(uint8_t*)((uint64_t) current_table + top) = item;
            top++;
        }
    }

    void Stack::push16(uint16_t item) {
        if (top + 2 > table_size - 1) {
            *(uint8_t*)((uint64_t) current_table + table_size - 1) = table_size - top;
            uintptr_t temp = malloc(table_size);
            *(uint64_t*)(temp) = (uint64_t) current_table;
            current_table = temp;
            top = 8;
            push16(item);
        } else {
            *(uint16_t*)((uint64_t) current_table + top) = item;
            top += 2;
        }
    }

    void Stack::push32(uint32_t item) {
        if (top + 4 > table_size - 1) {
            *(uint8_t*)((uint64_t) current_table + table_size - 1) = table_size - top;
            uintptr_t temp = malloc(table_size);
            *(uint64_t*)(temp) = (uint64_t) current_table;
            current_table = temp;
            top = 8;
            push32(item);
        } else {
            *(uint32_t*)((uint64_t) current_table + top) = item;
            top += 4;
        }
    }

    void Stack::push64(uint64_t item) {
        if (top + 8 > table_size - 1) {
            *(uint8_t*)((uint64_t) current_table + table_size - 1) = table_size - top;
            uintptr_t temp = malloc(table_size);
            *(uint64_t*)(temp) = (uint64_t) current_table;
            current_table = temp;
            top = 8;
            push64(item);
        } else {
            *(uint64_t*)((uint64_t) current_table + top) = item;
            top += 8;
        }
    }

}
