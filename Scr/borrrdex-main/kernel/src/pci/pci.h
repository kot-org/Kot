#pragma once

#include "drivers/modules.h"
#include <stdint.h>

static constexpr uint16_t INVALID_DEVICE = 0xFFFF;

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_interface;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
} __attribute__((packed)) pci_header_t;

typedef struct
{
    pci_header_t common;
    uint32_t bar[6];
    uint32_t cardbus_cis_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base;
    uint8_t capabilities_ptr;
    uint64_t reserved;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
} __attribute__((packed)) pci_device_t;


typedef struct {
    pci_header_t common;
    uint32_t bar[2];
    uint8_t primary_bus;
    uint8_t secondary_bus;
    uint8_t subordinate_bus;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    uint16_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_memory_base_upper;
    uint32_t prefetchable_memory_limit_upper;
    uint16_t io_base_upper;
    uint16_t io_limit_upper;
    uint8_t capabilities_pointer;
    uint64_t reserved:48;
    uint32_t expansion_rom_base;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;

} __attribute__((packed)) pci_to_pci_bridge_t;

pci_header_t* pci_get_device(void* cfgArea, uint8_t bus, uint8_t device, uint8_t function);
void pci_print_all_bar(pci_header_t* h);
void pci_print_bus(void* base_address, uint8_t bus);

// Remove these later and make modules
void* pci_find_type(void* cfgArea, int cls, int subclass, int prog_if);

#define PCI_MODULE_INIT(name, handler, classcode, subcode) \
    static pci_device_module_t __module_pci__##name \
    __attribute__((section("real_modules_ptr"))) = \
    {classcode, subcode, handler}; \
    MODULE_DEFINE(MODULE_TYPE_PCI, name, &__module_pci__##name)

extern "C" {
    typedef int(*pci_device_handler)(pci_header_t* );

    typedef struct {
        uint8_t classcode;
        uint8_t subclass;
        pci_device_handler device_handler;
    } pci_device_module_t;
}

