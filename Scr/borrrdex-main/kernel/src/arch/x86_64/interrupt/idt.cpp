#include "idt.h"
#include "arch/x86_64/gdt/gdt.h"
#include "arch/x86_64/irq.h"

static idt_desc_t s_idt_descriptors[IDT_MAX_INTERRUPTS];
static idt_t s_table;

typedef struct {
    uint16_t limit;
    uint32_t base;
} idt_32_t;


static idt_32_t s_table_32;

extern "C" void isr_handler0();
extern "C" void isr_handler1();
extern "C" void isr_handler2();
extern "C" void isr_handler3();
extern "C" void isr_handler4();
extern "C" void isr_handler5();
extern "C" void isr_handler6();
extern "C" void isr_handler7();
extern "C" void isr_handler8();
extern "C" void isr_handler9();
extern "C" void isr_handler10();
extern "C" void isr_handler11();
extern "C" void isr_handler12();
extern "C" void isr_handler13();
extern "C" void isr_handler14();
extern "C" void isr_handler15();
extern "C" void isr_handler16();
extern "C" void isr_handler17();
extern "C" void isr_handler18();
extern "C" void isr_handler19();
extern "C" void isr_default_handler();

void idt_init() {
    idt_install_gate(0, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler0);
    idt_install_gate(1, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler1);
    idt_install_gate(2, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler2);
    idt_install_gate(3, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler3);
    idt_install_gate(4, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler4);
    idt_install_gate(5, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler5);
    idt_install_gate(6, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler6);
    idt_install_gate(7, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler7);
    idt_install_gate(8, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler8);
    idt_install_gate(9, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler9);
    idt_install_gate(10, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler10);
    idt_install_gate(11, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler11);
    idt_install_gate(12, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler12);
    idt_install_gate(13, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler13);
    idt_install_gate(14, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler14);
    idt_install_gate(15, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler15);
    idt_install_gate(16, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler16);
    idt_install_gate(17, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler17);
    idt_install_gate(18, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler18);
    idt_install_gate(19, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_handler19);

    for(uint32_t i = 20; i < IDT_MAX_INTERRUPTS; i++) {
        idt_install_gate(i, IDT_DESC_PRESENT | IDT_DESC_TRAP32, GDT_SELECTOR_KERNEL_CODE, isr_default_handler);
    }

    s_table.limit = (uint16_t)(sizeof(idt_desc_t) * IDT_MAX_INTERRUPTS) - 1;
    s_table.base = (uint64_t)&s_idt_descriptors;

    s_table.limit = (uint16_t)(sizeof(idt_desc_t) * IDT_MAX_INTERRUPTS) - 1;
    s_table.base = (uint32_t)(uint64_t)&s_idt_descriptors;

    asm volatile("lidt (%%rax)" : : "a"((uint64_t)&s_table));
}

const void* idt_address() {
    return &s_table_32;
}

void idt_install_gate(uint32_t index, uint16_t flags, uint16_t selector, irq_handler irq) {
    if(index >= IDT_MAX_INTERRUPTS || !irq) {
        return;
    }

    uint64_t irq_base = (uint64_t)&(*irq);
    s_idt_descriptors[index].base_low = (uint16_t)(irq_base & 0xFFFF);
    s_idt_descriptors[index].base_mid = (uint16_t)((irq_base >> 16) & 0xFFFF);
    s_idt_descriptors[index].base_high = (uint32_t)((irq_base >> 32) & 0xFFFFFFFF);
    s_idt_descriptors[index].ist = 0;
    s_idt_descriptors[index].zero = 0;
    s_idt_descriptors[index].flags = (uint8_t)flags;
    s_idt_descriptors[index].selector = selector;
}