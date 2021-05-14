#pragma once

#include <stdint.h>

constexpr uint8_t IDT_DESC_TASK32   = 0x05; ///< 32-bit task gate
constexpr uint8_t IDT_DESC_INT16    = 0x06; ///< 16-bit interrupt gate
constexpr uint8_t IDT_DESC_TRAP16   = 0x07; ///< 16-bit trap gate
constexpr uint8_t IDT_DESC_INT32    = 0x0E; ///< 32-bit interrupt gate
constexpr uint8_t IDT_DESC_TRAP32   = 0x0F; ///< 32-bit trap gate (most common)

constexpr uint8_t IDT_DESC_RING1    = 0x40; ///< Ring 0 or 1 can use
constexpr uint8_t IDT_DESC_RING2    = 0x20; ///< Ring 0-2 can use
constexpr uint8_t IDT_DESC_RING3    = 0x60; ///< Ring 0-3 can use
constexpr uint8_t IDT_DESC_PRESENT  = 0x80; ///< Must be 1 on all valid entries

constexpr uint8_t IDT_MAX_INTERRUPTS = 0xFF; ///< The maximum number of interrupts supported

#ifdef __cplusplus
extern "C" {
#endif

    typedef void(*irq_handler)();

    typedef struct idt_descriptor {
        uint16_t base_low;  ///< The interrupt handler's address (bits 0 - 15)
        uint16_t selector;  ///< The selector of the code segment this interrupt function is in (basically always GDT_SELECTOR_KERNEL_CODE)
        uint8_t ist;        ///< Bits 0..2 hold interrupt stack table offset, rest zero
        uint8_t flags;      ///< The flags on this entry (see IDT_DESC_*)
        uint16_t base_mid;  ///< The interrupt handler's address (bits 16 - 31)
        uint32_t base_high; ///< The interrupt handler's address (bits 32 - 63);
        uint32_t zero;      ///< Reserved
    } __attribute__((packed)) idt_desc_t;

    // Basically, this structure is an array of the above,
    // defined as a block of memory
    typedef struct id_table {
        uint16_t limit;     ///< The size in memory of all the idt_desc_t entries (laid out sequentially)
        uint64_t base;      ///< The memory address of the first idt_desc_t entry 
    } __attribute__((packed)) idt_t;

    /**
     * Sets up the IDT in the CPU.  Don't call this more than once.
     */
    void idt_init();

    const void* idt_address();

    /**
     * Sets the specified hardware interrupt signal to use the corresponding handler
     * @param index     The IRQ number
     * @param flags     The flags on this entry (see IDT_DESC_*)
     * @param selector  The selector of the code segment this interrupt function is in (basically always GDT_SELECTOR_KERNEL_CODE)
     * @param irq       The handler for the IRQ
     */
    void idt_install_gate(uint32_t index, uint16_t flags, uint16_t selector, irq_handler irq);

#ifdef __cplusplus
}
#endif