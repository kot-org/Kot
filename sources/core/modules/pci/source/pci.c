#include <stdint.h>
#include <stdbool.h>

#include <main.h>
#include <list.h>

#include <arch/include.h>
#include ARCH_INCLUDE(asm.h)

#define PCI_CONFIG_ADDR     0xCF8
#define PCI_CONFIG_DATA     0xCFC

#define PCI_CONFIGURATION_SPACE_SIZE 0x100

static void pci_write32(uint32_t addr, uint32_t data) {
    addr &= ~(0b11);
    /* write address */
    io_write32(PCI_CONFIG_ADDR, addr);
    /* write data */
    io_write32(PCI_CONFIG_DATA, data);
}

static uint32_t pci_read32(uint32_t addr) {
    addr &= ~(0b11);
    /* write address */
    io_write32(PCI_CONFIG_ADDR, addr);
    /* read data */
    return io_read32(PCI_CONFIG_DATA);
}

static uint16_t pci_read16(uint32_t addr) {
    uint8_t offset = addr & 0xff;
    addr &= ~(0b11);
    return (uint16_t) ((pci_read32(addr) >> ((offset & 0b10) * 0x10)) & 0xffff);
}

static uint8_t pci_read8(uint32_t addr) {
    uint8_t offset = addr & 0xff;
    addr &= ~(0b11);
    return (uint16_t) ((pci_read16(addr) >> ((offset & 0b1) * 0x8)) & 0xff);
}

static void pci_memcpy8(void* dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x1){
        *(uint8_t*)((uint64_t)dst + i) = pci_read8(src + i);
    }
}

static void pci_memcpy16(void* dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x2){
        *(uint16_t*)((uint64_t)dst + i) = pci_read16(src + i);
    }
}

static void pci_memcpy32(void* dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x4){
        *(uint32_t*)((uint64_t)dst + i) = pci_read32(src + i);
    }
}

static uint32_t pci_device_base_address(uint16_t bus, uint16_t device, uint16_t func){
    return (uint32_t) ((1 <<  31) | (bus << 16) | (device << 11) | (func << 8));
}

static bool check_device_addr(uint32_t addr){
    uint16_t vendor_id = pci_read16(addr + PCI_VENDOR_ID_OFFSET);
    if(vendor_id == 0xffff) return false;
    return true;
}

static bool check_device(uint16_t bus, uint16_t device, uint16_t func){
    uint32_t addr = pci_device_base_address(bus, device, func);
    return check_device_addr(addr);
}

void receive_configuration_space_pci(pci_device_t* device){
    pci_memcpy32(device->configuration_space, device->address, PCI_CONFIGURATION_SPACE_SIZE);
}

void send_configuration_space_pci(pci_device_t* device){
    uint64_t back_buffer = (uint64_t)device->configuration_space_back;
    uint64_t front_buffer = (uint64_t)device->configuration_space;
    uint64_t pci_address = device->address;

    // align buffer
    back_buffer &= ~(0b11);
    front_buffer &= ~(0b11);

    for(size_t i = 0; i < PCI_CONFIGURATION_SPACE_SIZE; i += 4){ // increment 32 bits
        if(*(uint32_t*)back_buffer != *(uint32_t*)front_buffer){
            pci_write32(pci_address, *(uint32_t*)front_buffer);
        }
        pci_address += 4;
        back_buffer += 4;
        front_buffer += 4;
    }

    // update back buffer
    memcpy((void*)back_buffer, (void*)front_buffer, PCI_CONFIGURATION_SPACE_SIZE);
}

static void get_device(pci_device_list_info_t* pci_device_list, uint16_t bus, uint16_t device, uint16_t func){
    uint32_t address = pci_device_base_address(bus, device, func);
    if(!check_device_addr(address)) return; 

    pci_device_t* device_info = (pci_device_t*)malloc(sizeof(pci_device_t));
    device_info->is_pcie = false;
    device_info->address = address; // this is not pci device, it's pcie device
    device_info->configuration_space = malloc(PCI_CONFIGURATION_SPACE_SIZE);
    device_info->configuration_space_back = malloc(PCI_CONFIGURATION_SPACE_SIZE);
    device_info->receive_configuration_space = &receive_configuration_space_pci;
    device_info->send_configuration_space = &send_configuration_space_pci;

    pci_memcpy32(device_info->configuration_space_back, device_info->address, PCI_CONFIGURATION_SPACE_SIZE);
    memcpy(device_info->configuration_space, device_info->configuration_space_back, PCI_CONFIGURATION_SPACE_SIZE);

    add_pci_device(pci_device_list, device_info);

    return;
}

void init_pci(pci_device_list_info_t* pci_device_list){
    for(uint32_t bus = 0; bus < 256; bus++) {
        for(uint32_t device = 0; device < 32; device++) {

            uint32_t addr = pci_device_base_address(bus, device, 0);
            uint16_t vendor_id = pci_read16(addr + PCI_VENDOR_ID_OFFSET);
            
            if(vendor_id == 0xffff) continue;

            uint8_t header_type = pci_read8(addr + PCI_HEADER_TYPE_OFFSET);

            if((header_type & 0x80) != 0){
                for(uint32_t func = 0; func < 8; func++) {
                    get_device(pci_device_list, bus, device, func);
                }
            }else{
                get_device(pci_device_list, bus, device, 0);
            }
        }
    }    
}
