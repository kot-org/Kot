#ifndef _AMD64_IDT_H
#define _AMD64_IDT_H 1

#include <stdint.h>
#include <global/pmm.h>

#define IDT_ENTRY_COUNT (PAGE_SIZE / sizeof(idt_entry_t))

enum
{
    IDT_TRAP = 0xeF,
    IDT_USER = 0x60,
    IDT_GATE = 0x8e,
};

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;

typedef struct {
    uint16_t offset_low;
    uint16_t code_segment;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    idt_entry_t entries[IDT_ENTRY_COUNT];
} __attribute__((packed)) idt_t;

void idt_init_cpu(void);
void idt_init(void);
void idt_update(idtr_t* idtr);

#endif // _AMD64_IDT_H
