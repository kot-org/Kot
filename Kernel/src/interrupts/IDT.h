#pragma once
#include <stdint.h>
#include "../lib/stdio.h"

#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_CallGate         0b10001100
#define IDT_TA_TrapGate         0b10001111

/* variables */
struct IDTDescEntry{
    uint16_t offset0;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore0;
    void SetOffset(uint64_t offset);
    uint64_t GetOffset();
};

struct IDTR{
    uint16_t Limit;
    uint64_t Offset;
}__attribute__((packed));

/* functions */
void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector, IDTR idtrl);