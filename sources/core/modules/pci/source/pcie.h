#ifndef _MODULE_PCI_PCIE_H
#define _MODULE_PCI_PCIE_H

#include <stdint.h>
#include <arch/include.h>
#include ARCH_INCLUDE(acpi.h)

#define PCIE_CONFIGURATION_SPACE_SIZE 0x1000

typedef struct{
    uint64_t base_address;
    uint16_t pci_seg_group;
    uint8_t start_bus;
    uint8_t end_bus;
    uint32_t reserved;
}__attribute__((packed)) device_config_t;

typedef struct{
    struct acpi_sdt_header header;
    uint64_t reserved;
    device_config_t configuration_space[];
}__attribute__((packed)) mcfg_header_t;

#endif // _MODULE_PCI_PCIE_H