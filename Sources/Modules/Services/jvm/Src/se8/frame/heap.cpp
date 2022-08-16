#include "heap.h"

namespace SE8 {

    Locals::Locals(uint64_t maxLocals) {
        tableSize = maxLocals;
        tables = vector_create();
    }

    uintptr_t Locals::provideTable(uint64_t tableIndex) {
        uintptr_t table = NULL;
        if (tables->length >= tableIndex) {
            table = vector_get(tables, tableIndex);
        } else {
            vector_expand(tables, tableIndex - tables->length);
        }
        if (table == NULL) {
            table = malloc(tableSize * 5);
            vector_set(tables, tableIndex, table);
        }
        return table;
    }

    uint64_t Locals::getPtr(uint64_t index) {
        uint64_t tableIndex = index / tableSize;
        uint64_t idx = (index - (tableSize * tableIndex)) * 5;
        uintptr_t table = provideTable(tableIndex);
        return (uint64_t) table + idx;
    }

    void Locals::setType(uint64_t ptr, uint8_t type) {
        *(uint8_t*)((uint64_t) ptr) = type;
    }

    uint8_t Locals::getType(uint64_t ptr) {
        return *(uint8_t*)((uint64_t) ptr);
    }

    void Locals::set32(uint64_t ptr, uint32_t value) {
        *(uint32_t*)((uint64_t) ptr + 1) = value;
    }

    void Locals::set64(uint64_t ptr, uint64_t value) {
        *(uint64_t*)((uint64_t) ptr + 1) = value;
    }

    uint32_t Locals::get32(uint64_t ptr) {
        return *(uint32_t*)((uint64_t) ptr + 1);
    }

    uint64_t Locals::get64(uint64_t ptr) {
        return *(uint64_t*)((uint64_t) ptr + 1);
    }

}