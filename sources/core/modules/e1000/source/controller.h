#ifndef _MODULE_E1000_CONTROLLER_H
#define _MODULE_E1000_CONTROLLER_H

#include <errno.h>
#include <stdint.h>
#include <e1000_regs.h>
#include <e1000x_regs.h>
#include <global/modules.h>
#include <global/hw_interrupt.h>

#define PACKET_SIZE PAGE_SIZE

typedef struct{
    bool is_bar_io;
    union{
        uint16_t io;
        void* memory;
    }bar_base;

    net_device_t* net_device;

    spinlock_t tx_desc_lock;
    void* tx_desc_physical;
    struct e1000_tx_desc* tx_desc;
    size_t tx_desc_count;

    spinlock_t rx_desc_lock;
    void* rx_desc_physical;
    struct e1000_rx_desc* rx_desc;
    size_t rx_desc_count;

    int interrupt_vector;
}e1000_controller_t;


#endif // _MODULE_E1000_CONTROLLER_H