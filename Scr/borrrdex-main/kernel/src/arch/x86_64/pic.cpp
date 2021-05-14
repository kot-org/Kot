#include "pic.h"
#include "io/io.h"
#include "acpi/apic.h"

static void* lapicAddr = nullptr;

extern "C" void __enable_apic();

void pic_sendcommand(uint8_t pic, uint8_t command) {
    if(pic > 1) {
        return;
    }

    uint16_t reg = pic == PIC_SECONDARY ? (uint16_t)PIC1_COMMAND_REGISTER : (uint16_t)PIC0_COMMAND_REGISTER;
    port_write_8(reg, command);
}

void pic_senddata(uint8_t pic, uint8_t data) {
    if(pic > 1) {
        return;
    }

    uint16_t reg = pic == PIC_SECONDARY ? (uint16_t)PIC1_INT_MASK_REGISTER : (uint16_t)PIC0_INT_MASK_REGISTER;
    port_write_8(reg, data);
}

extern "C" void pic_init(uint8_t mask1, uint8_t mask2) {
    // Don't let interrupts come while we do this
    asm volatile("cli");

    // Setup the control word
    uint8_t icw = PIC_ICW1_MASK_INIT | PIC_ICW1_MASK_IC4;

    // Start the process of initializing, now the PICs will expect
    // the sequence of words that follows
    pic_sendcommand(PIC_PRIMARY, icw);
    pic_sendcommand(PIC_SECONDARY, icw);

    // Remap them so that the IRQ do not overlap with hardwired
    // hardware exception interrupts.  After this IRQs will be
    // offset by 32 (0 - 15 -> 32 - 47)
    pic_senddata(PIC_PRIMARY, 0x20);
    pic_senddata(PIC_SECONDARY, 0x28);

    // Next control word, sets PIC1 as a slave of PIC0
    // After this all of PIC1 IRQ will flow through
    // PIC0 IRQ 2
    pic_senddata(PIC_PRIMARY, 0x04);
    pic_senddata(PIC_SECONDARY, 0x02);

    // Final control word (4), enable i86 mode
    pic_senddata(PIC_PRIMARY, PIC_ICW4_MASK_UPM);
    pic_senddata(PIC_SECONDARY, PIC_ICW4_MASK_UPM);

    // Activate IRQs according to the mask
    pic_senddata(PIC_PRIMARY, mask1);
    pic_senddata(PIC_SECONDARY, mask2);

    // Turn interrupts back on
    asm volatile("sti");

    pic_eoi(8);
}

extern "C" void pic_override(void* lapic) {
    port_write_8(PIC0_INT_MASK_REGISTER, 0xFF);
    port_write_8(PIC1_INT_MASK_REGISTER, 0xFF);

    port_write_8(CHIPSET_ADDRESS_REGISTER, IMCR_REGISTER_ADDRESS);
    port_write_8(CHIPSET_DATA_REGISTER, IMCR_VIA_APIC);

    lapicAddr = lapic;
}

extern "C" void pic_mask_interrupt(uint8_t irq) {
    uint16_t port;
    if(irq < 8) {
        port = PIC0_INT_MASK_REGISTER;
    } else {
        port = PIC1_INT_MASK_REGISTER;
        irq -= 8;
    }

    uint8_t value = port_read_8(port) | (1 << irq);
    port_write_8(port, value);
}

extern "C" void pic_unmask_interrupt(uint8_t irq) {
    uint16_t port;
    if(irq < 8) {
        port = PIC0_INT_MASK_REGISTER;
    } else {
        port = PIC1_INT_MASK_REGISTER;
        irq -= 8;
    }

    uint8_t value = port_read_8(port) & ~(1 << irq);
    port_write_8(port, value);
}

extern "C" void pic_eoi(uint8_t irq) {
    if(lapicAddr) {
        auto* eoiAddr = (volatile uint32_t *)((uint64_t)lapicAddr + lapic::REG_OFFSET_EOI);
        *eoiAddr = 0;
    } else {
        if(irq >= 8) {
            port_write_8(PIC1_COMMAND_REGISTER, 0x20);
        }

        port_write_8(PIC0_COMMAND_REGISTER, 0x20);
    }
}