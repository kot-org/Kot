#ifndef _MODULE_PS2_CORE_H
#define _MODULE_PS2_CORE_H

#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <lib/assert.h>
#include <global/modules.h>
#include <global/hw_interrupt.h>

#include <arch/include.h>
#include ARCH_INCLUDE(asm.h)
#include ARCH_INCLUDE(interrupts.h)

#define PS2_PORT_COUNT              0x2

#define PS2_IRQ_PORT1               0x1
#define PS2_IRQ_PORT2               0xC

#define PS2_DATA                    0x60
#define PS2_STATUS                  0x64
#define PS2_COMMAND                 0x64

#define PS2_ACK                     0xFA
#define PS2_RESEND                  0xFE

#define PS2_TYPE_UNKNOW             0x0
#define PS2_TYPE_MOUSE              0x1
#define PS2_TYPE_MOUSE_SCROLL       0x2
#define PS2_TYPE_MOUSE_5BUTTONS     0x3
#define PS2_TYPE_KEYBOARD           0x4

#define PS2_STATU_OUPUTBUFFER       (1 << 0)
#define PS2_STATU_INPUTBUFFER       (1 << 1)
#define PS2_STATU_SYSTEMFALG        (1 << 2)
#define PS2_STATU_COMMANDDATA       (1 << 3)
#define PS2_STATU_KEYBOARDLOCK      (1 << 4)
#define PS2_STATU_SECONDBUFFERFULL  (1 << 5)
#define PS2_STATU_TIMEOUTERROR      (1 << 6)
#define PS2_STATU_PARTYERROR        (1 << 7)

#define BIT_SETV(a,b,c) ((a) |= ((c)<<(b)))

typedef struct{
    bool is_present;
    uint8_t type;
    uint8_t irq;
    uint8_t port_number;
    void (*interrupt_handler)(uint8_t);
} ps2_port_t;

void disable_ports(void);
void enable_ports(void);
int ports_initalize(void);
int ps2_interrupt_handler(int id);
void ps2_send_command(uint8_t command);
uint8_t ps2_get_status(void);
void ps2_send_data(uint8_t data);
uint8_t ps2_get_data(void);
void ps2_wait_output(void);
void ps2_wait_input(void);
void ps2_send_data_port1(uint8_t data);
void ps2_send_data_port2(uint8_t data);
uint8_t ps2_configuration_get(void);
void ps2_configuration_set(uint8_t data);
uint8_t ps2_controller_output_get(void);
void ps2_controller_output_set(uint8_t data);
void ps2_send_data_port(ps2_port_t* port, uint8_t data);

#endif // _MODULE_PS2_CORE_H