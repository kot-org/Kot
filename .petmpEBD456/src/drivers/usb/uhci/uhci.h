#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../../lib/stdio.h"
#include "../../../paging/PageTableManager.h"
#include "../../../memory/heap.h"
#include "../../../hardware/pci/pci.h"
#include "../../../io/io.h"
#include "../../../paging/PageFrameAllocator.h"

#define CMD_MAX_PACKET 1 << 7
#define CMD_CONFIG_DONE 1 << 6
#define CMD_SOFT_DEBUG 1 << 5
#define CMD_GLOBAL_RESUME 1 << 4
#define CMD_GLOBAL_SUSPEND 1 << 3
#define CMD_GLOBAL_RESET 1 << 2
#define CMD_HOST_CTRL_RESET 1 << 1
#define CMD_RUN_STOP 1 << 0

#define PORT_CONNECTION                 (1 << 0)    // Current Connect Status
#define PORT_CONNECTION_CHANGE          (1 << 1)    // Connect Status Change
#define PORT_ENABLE                     (1 << 2)    // Port Enabled
#define PORT_ENABLE_CHANGE              (1 << 3)    // Port Enable Change
#define PORT_LS                         (3 << 4)    // Line Status
#define PORT_RD                         (1 << 6)    // Resume Detect
#define PORT_LSDA                       (1 << 8)    // Low Speed Device Attached
#define PORT_RESET                      (1 << 9)    // Port Reset
#define PORT_SUSP                       (1 << 12)   // Suspend
#define PORT_RWC                        (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE)

#define REG_CMD                         0x00        // USB Command
#define REG_STS                         0x02        // USB Status
#define REG_INTR                        0x04        // USB Interrupt Enable
#define REG_FRNUM                       0x06        // Frame Number
#define REG_FRBASEADD                   0x08        // Frame List Base Address
#define REG_SOFMOD                      0x0C        // Start of Frame Modify
#define REG_PORT1                       0x10        // Port 1 Status/Control
#define REG_PORT2                       0x12        // Port 2 Status/Control
#define REG_LEGSUP                      0xc0        // Legacy Support

namespace UHCI{
    struct UHCI_REGISTERS {
        uint16_t USBCMD;
        uint16_t USBSTS;
        uint16_t USBINTR;
        uint16_t FRNUM;
        uint32_t FRBASEADD;
        uint8_t SOFMOD;
        uint16_t PORTSC1;
        uint16_t PORTSC2;
    }__attribute__((packed));

    void InitializeUHCI(PCI::PCIDeviceHeader* device);
    void PortClear(uint64_t port, uint16_t data);
    void PortSet(uint64_t port, uint16_t data);
}
