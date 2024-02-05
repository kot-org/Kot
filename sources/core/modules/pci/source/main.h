#ifndef _MODULE_PCI_MAIN_H
#define _MODULE_PCI_MAIN_H

#include <stdint.h>
#include <stdbool.h>
#include <lib/vector.h>

#define PCI_CONFIGURATION_SPACE_PCI     0x100
#define PCI_CONFIGURATION_SPACE_PCIE    0x1000

#define PCI_VENDOR_ID_OFFSET 		0x0
#define PCI_DEVICE_ID_OFFSET 		0x2
#define PCI_COMMAND_OFFSET 			0x4
#define PCI_STATUS_OFFSET 			0x6
#define PCI_REVISION_ID_OFFSET 		0x8
#define PCI_PROG_IF_OFFSET 			0x9
#define PCI_SUBCLASS_OFFSET 		0xA
#define PCI_CLASS_CODE_OFFSET 		0xB
#define PCI_CACHE_SIZE_OFFSET 		0xC
#define PCI_LATENCY_TIMER_OFFSET 	0xD
#define PCI_HEADER_TYPE_OFFSET 		0xE
#define PCI_BIST_OFFSET 			0xF

#define PCI_COMMAND_IO_SPACE            (1 << 0x0)
#define PCI_COMMAND_MEMORY_SPACE        (1 << 0x1)
#define PCI_COMMAND_BUS_MASTERING       (1 << 0x2)
#define PCI_COMMAND_INTERRUPT_DISABLE   (1 << 0xA)

/* PCI header */

typedef struct{
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t sub_class_id;
    uint8_t class_id;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
}__attribute__((packed)) pci_device_header_t;

typedef struct{
    pci_device_header_t header;
    uint32_t bar[6];
    uint32_t card_bus_cis_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base_addr;
    uint8_t capabilities_ptr;
    uint8_t  reserved0;
    uint16_t reserved1;
    uint32_t reserved2;
    uint8_t intrerupt_line;
    uint8_t intrerupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
}__attribute__((packed)) pci_header0_t;

typedef struct{
    pci_device_header_t header;
    /* todo */
}__attribute__((packed)) pci_header1_t;

/* capabilities */

enum pci_capabilities{
    pci_capabilities_msi = 0x5,
    pci_capabilities_msix = 0x11,
};

typedef struct{
    uint16_t control;
    uint64_t address;
    uint16_t data;
    uint16_t reserved;
    uint32_t mask;
    uint32_t pending;
}__attribute__((packed)) pci_capability_msi_t;

typedef struct{
    uint64_t address;
    uint32_t data;
    uint32_t control;
}__attribute__((packed)) pcimsix_table_t;

typedef struct{
    uint16_t control;
    uint8_t bir:2;
    uint32_t table_offset:30;
    uint8_t pending_bit_bir:2;
    uint32_t pending_bit_offset:30;
}__attribute__((packed)) pci_capability_msix_t;

typedef struct{
    uint8_t capability_id;
    uint8_t capability_next;

    union{
        pci_capability_msi_t msi;
        pci_capability_msix_t msix;
    };
}__attribute__((packed)) pci_capability_t;


typedef struct pci_device_t{
    bool is_pcie;
    uint64_t address; // pci only
    void* configuration_space_back; // pci only
    void* configuration_space;

    void (*receive_configuration_space)(struct pci_device_t* device);
    void (*send_configuration_space)(struct pci_device_t* device);
}pci_device_t;

typedef struct{
    vector_t* devices;
}pci_device_list_info_t;

typedef struct{
    pci_device_t** devices;
    size_t devices_count;
}pci_device_array_info_t;

#endif // _MODULE_PCI_MAIN_H