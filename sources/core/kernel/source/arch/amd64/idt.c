#include <arch/include.h>
#include ARCH_INCLUDE(idt.h)
#include ARCH_INCLUDE(gdt.h)
#include ARCH_INCLUDE(interrupts.h)

extern void* __interrupt_vector[IDT_ENTRY_COUNT];

static __attribute__((aligned(PAGE_SIZE))) idt_t _idt = {};

static idtr_t _idtr = {
    .limit = sizeof(idt_t) - 1,
    .base = (uint64_t) &_idt,
};

idt_entry_t idt_entry(void* handler, uint8_t ist, uint8_t idt_flags) {
    return (idt_entry_t) {
        .offset_low = ((uint64_t)handler) & 0xffff,
        .code_segment = GDT_KERNEL_CODE * sizeof(gdt_entry_t),
        .ist = ist,
        .attributes = idt_flags,
        .offset_middle = ((uint64_t)handler >> 16) & 0xffff,
        .offset_high = ((uint64_t)handler >> 32) & 0xffffffff,
        .zero = 0,
    };
}

void idt_init_cpu(void){
    idt_update(&_idtr);
}

void idt_init(void) {
    for (uint16_t i = 0; i < 256; i++) {
        _idt.entries[i] = idt_entry(__interrupt_vector[i], 0, IDT_GATE);
    }
    idt_init_cpu();
}