#pragma once

#include <stdint.h>

// From PIC0
constexpr uint8_t PIC_IRQ_TIMER         = 0x00;
constexpr uint8_t PIC_IRQ_KEYBOARD      = 0x01;
constexpr uint8_t PIC_IRQ_SERIAL2       = 0x03;
constexpr uint8_t PIC_IRQ_SERIAL1       = 0x04;
constexpr uint8_t PIC_IRQ_PARALLEL2     = 0x05;
constexpr uint8_t PIC_IRQ_FLOPPYDISK    = 0x06;
constexpr uint8_t PIC_IRQ_PARALLEL1     = 0x07;

// From PIC1
constexpr uint8_t PIC_IRQ_CMOSTIMER     = 0x08;
constexpr uint8_t PIC_IRQ_CGARETRACE    = 0x09;
constexpr uint8_t PIC_IRQ_PS2MOUSE      = 0x0C;
constexpr uint8_t PIC_IRQ_PRIMARYHD     = 0x0D;
constexpr uint8_t PIC_IRQ_SECONDARYHD   = 0x0E;

constexpr uint8_t PIC_OCW2_MASK_L1      = 0x01;
constexpr uint8_t PIC_OCW2_MASK_L2      = 0x02;
constexpr uint8_t PIC_OCW2_MASK_L3      = 0x04;
constexpr uint8_t PIC_OCW2_MASK_EOI     = 0x20;
constexpr uint8_t PIC_OCW2_MASK_SL      = 0x40;
constexpr uint8_t PIC_OCW2_MASK_ROTATE  = 0x80;

constexpr uint8_t PIC_OCW3_MASK_RIS     = 0x01;
constexpr uint8_t PIC_OCW3_MASK_RIR     = 0x02;
constexpr uint8_t PIC_OCW3_MASK_MODE    = 0x04;
constexpr uint8_t PIC_OCW3_MASK_SMM     = 0x20;
constexpr uint8_t PIC_OCW3_MASK_ESMM    = 0x40;
constexpr uint8_t PIC_OCW3_MASK_D7      = 0x80;

constexpr uint8_t PIC0_COMMAND_REGISTER     = 0x20;
constexpr uint8_t PIC0_INT_MASK_REGISTER    = 0x21;

constexpr uint8_t PIC1_COMMAND_REGISTER     = 0xA0;
constexpr uint8_t PIC1_INT_MASK_REGISTER    = 0xA1;

// Control Word 1
constexpr uint8_t PIC_ICW1_MASK_IC4     = 0x01;
constexpr uint8_t PIC_ICW1_MASK_SNGL    = 0x02;
constexpr uint8_t PIC_ICW1_MASK_ADI     = 0x04;
constexpr uint8_t PIC_ICW1_MASK_LTIM    = 0x08;
constexpr uint8_t PIC_ICW1_MASK_INIT    = 0x10;

// Control Word 4
constexpr uint8_t PIC_ICW4_MASK_UPM     = 0x01;
constexpr uint8_t PIC_ICW4_MASK_AEOI    = 0x02;
constexpr uint8_t PIC_ICW4_MASK_MS      = 0x04;
constexpr uint8_t PIC_ICW4_MASK_BUF     = 0x08;
constexpr uint8_t PIC_ICW4_MASK_SFNM    = 0x10;

constexpr uint8_t PIC_PRIMARY = 0x00;
constexpr uint8_t PIC_SECONDARY = 0x01;

#ifdef __cplusplus
extern "C" {
#endif

    void pic_init(uint8_t mask1, uint8_t mask2);
    void pic_override(void* lapic);
    void pic_mask_interrupt(uint8_t irq);
    void pic_unmask_interrupt(uint8_t irq);
    void pic_eoi(uint8_t irq);

#ifdef __cplusplus
}
#endif