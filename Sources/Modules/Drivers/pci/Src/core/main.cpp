#include <core/main.h>

uint32_t PCIDeviceBaseAddress(uint16_t bus, uint16_t device, uint16_t func){
    return (uint32_t) ((1 <<  31) | (bus << 16) | (device << 11) | (func << 8));
}

uint32_t PCIRead32(uint32_t addr) {
    addr &= ~(0b11);
    /* Write address */
    IoWrite32(PCI_CONFIG_ADDR, addr);
    /* Read data */
    return IoRead32(PCI_CONFIG_DATA);
}

void PCIWrite32(uint32_t addr, uint32_t data) {
    addr &= ~(0b11);
    /* Write address */
    IoWrite32(PCI_CONFIG_ADDR, addr);
    /* Read data */
    IoWrite32(PCI_CONFIG_DATA, data);
}

uint16_t PCIRead16(uint32_t addr) {
    uint8_t offset = addr & 0xff;
    addr &= ~(0b11);
    return (uint16_t) ((PCIRead32(addr) >> ((offset & 0b10) * 0x10)) & 0xffff);
}

uint8_t PCIRead8(uint32_t addr) {
    uint8_t offset = addr & 0xff;
    addr &= ~(0b11);
    return (uint16_t) ((PCIRead16(addr) >> ((offset & 0b1) * 0x8)) & 0xff);
}

void PCIMemcpyToMemory8(uintptr_t dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x1){
        *(uint8_t*)((uint64_t)dst + i) = PCIRead8(src + i);
    }
}

void PCIMemcpyToMemory16(uintptr_t dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x2){
        *(uint16_t*)((uint64_t)dst + i) = PCIRead16(src + i);
    }
}

void PCIMemcpyToMemory32(uintptr_t dst, uint32_t src, size_t size){
    src &= ~(0b11);
    for(size_t i = 0; i < size; i += 0x4){
        *(uint32_t*)((uint64_t)dst + i) = PCIRead32(src + i);
    }
}

uintptr_t GetDevice(uint16_t bus, uint16_t device, uint16_t func){
    uint32_t Addr = PCIDeviceBaseAddress(bus, device, func);
    uint8_t VendorID = PCIRead8(Addr + PCI_VENDOR_ID_OFFSET);
    if(VendorID == 0xffff) return NULL;
    uint8_t HeaderType = PCIRead8(Addr + PCI_HEADER_TYPE_OFFSET);
    uintptr_t Header = NULL;
    switch (HeaderType){
    case 0x0:
        Header = malloc(sizeof(PCIHeader0));
        PCIMemcpyToMemory32(Header, Addr, sizeof(PCIHeader0));
        char buffer[100];
        char buffernum[33];
        *buffer = NULL;
        strcat(buffer, "[PCI] Vendor: 0x");
        itoa(((PCIHeader0*)Header)->Header.VendorID, buffernum, 16);
        strcat(buffer, buffernum);
        strcat(buffer, " Device: 0x");
        itoa(((PCIHeader0*)Header)->Header.DeviceID, buffernum, 16);
        strcat(buffer, buffernum);
        strcat(buffer, " Class: 0x");
        itoa(((PCIHeader0*)Header)->Header.Class, buffernum, 16);
        strcat(buffer, buffernum);
        strcat(buffer, " Subclass: 0x");
        itoa(((PCIHeader0*)Header)->Header.Subclass, buffernum, 16);
        strcat(buffer, buffernum);
        strcat(buffer, " ProgIF: 0x");
        itoa(((PCIHeader0*)Header)->Header.ProgIF, buffernum, 16);
        strcat(buffer, buffernum);
        Printlog(buffer);
        break;
    case 0x1:
        /* TODO */
        Printlog("[Error] PCI-to-PCI bridge not supported");
        break;
    default:
    return 0;
        Printlog("[Error] Unknow header type");
        break;
    }
    return Header;
}

void EnumerateDevices() {
    for(uint32_t bus = 0; bus < 256; bus++) {
        for(uint32_t device = 0; device < 32; device++) {
            uint32_t Addr = PCIDeviceBaseAddress(bus, device, NULL);
            uint8_t vendorID = PCIRead8(Addr + PCI_VENDOR_ID_OFFSET);
            if(vendorID == 0xffff) continue;
            uint8_t headerType = PCIRead8(Addr + PCI_HEADER_TYPE_OFFSET);
            if(headerType & 0x80 != 0){
                for(uint32_t func = 0; func < 8; func++) {
                    GetDevice(bus, device, func);
                }
            }else{
                GetDevice(bus, device, NULL);
            }
        }
    }    
}

extern "C" int main(int argc, char* argv[]) {
    Printlog("[PCI] Initialization ...");

    EnumerateDevices();

    return KSUCCESS;
}
