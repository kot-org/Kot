#include <pci/pci.h>

void PCIWrite32(uint32_t addr, uint32_t data) {
    addr &= ~(0b11);
    /* Write address */
    kot_IoWrite32(PCI_CONFIG_ADDR, addr);
    /* Write data */
    kot_IoWrite32(PCI_CONFIG_DATA, data);
}

uint32_t PCIRead32(uint32_t addr) {
    addr &= ~(0b11);
    /* Write address */
    kot_IoWrite32(PCI_CONFIG_ADDR, addr);
    /* Read data */
    return kot_IoRead32(PCI_CONFIG_DATA);
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

void PCIMemcpy8(void* dst, uint32_t src, size64_t size){
    src &= ~(0b11);
    for(size64_t i = 0; i < size; i += 0x1){
        *(uint8_t*)((uint64_t)dst + i) = PCIRead8(src + i);
    }
}

void PCIMemcpy16(void* dst, uint32_t src, size64_t size){
    src &= ~(0b11);
    for(size64_t i = 0; i < size; i += 0x2){
        *(uint16_t*)((uint64_t)dst + i) = PCIRead16(src + i);
    }
}

void PCIMemcpy32(void* dst, uint32_t src, size64_t size){
    src &= ~(0b11);
    for(size64_t i = 0; i < size; i += 0x4){
        *(uint32_t*)((uint64_t)dst + i) = PCIRead32(src + i);
    }
}

uint32_t PCIDeviceBaseAddress(uint16_t bus, uint16_t device, uint16_t func){
    return (uint32_t) ((1 <<  31) | (bus << 16) | (device << 11) | (func << 8));
}

bool CheckDevice(uint32_t Addr){
    uint16_t VendorID = PCIRead16(Addr + PCI_VENDOR_ID_OFFSET);
    if(VendorID == 0xffff) return false;
    return true;
}

bool CheckDevice(uint16_t bus, uint16_t device, uint16_t func){
    uint32_t Addr = PCIDeviceBaseAddress(bus, device, func);
    return CheckDevice(Addr);
}

void GetDevice(struct PCIDeviceListInfo_t* PCIDeviceList, uint16_t bus, uint16_t device, uint16_t func){
    uint32_t Address = PCIDeviceBaseAddress(bus, device, func);
    if(!CheckDevice(Address)) return; 

    PCIDevice_t* Device = (PCIDevice_t*)malloc(sizeof(PCIDevice_t));
    Device->IsPCIe = false;
    Device->Address = Address; // this is not pci device, it's pcie device
    Device->ConfigurationSpace = malloc(PCI_CONFIGURATION_SPACE_SIZE);
    Device->ConfigurationSpaceBack = malloc(PCI_CONFIGURATION_SPACE_SIZE);

    PCIMemcpy32(Device->ConfigurationSpaceBack, Device->Address, PCI_CONFIGURATION_SPACE_SIZE);
    memcpy(Device->ConfigurationSpace, Device->ConfigurationSpaceBack, PCI_CONFIGURATION_SPACE_SIZE);

    AddPCIDevice(PCIDeviceList, Device);

    return;
}

void InitPCI(struct PCIDeviceListInfo_t* PCIDeviceList){
    for(uint32_t bus = 0; bus < 256; bus++) {
        for(uint32_t device = 0; device < 32; device++) {

            uint32_t Addr = PCIDeviceBaseAddress(bus, device, NULL);
            uint16_t vendorID = PCIRead16(Addr + PCI_VENDOR_ID_OFFSET);
            
            if(vendorID == 0xffff) continue;

            uint8_t headerType = PCIRead8(Addr + PCI_HEADER_TYPE_OFFSET);

            if((headerType & 0x80) != 0){
                for(uint32_t func = 0; func < 8; func++) {
                    GetDevice(PCIDeviceList, bus, device, func);
                }
            }else{
                GetDevice(PCIDeviceList, bus, device, NULL);
            }
        }
    }    
}

void ReceiveConfigurationSpacePCI(struct PCIDevice_t* Device){
    PCIMemcpy32(Device->ConfigurationSpace, Device->Address, PCI_CONFIGURATION_SPACE_SIZE);
}

void SendConfigurationSpacePCI(struct PCIDevice_t* Device){
    uint64_t BackBuffer = (uint64_t)Device->ConfigurationSpaceBack;
    uint64_t FrontBuffer = (uint64_t)Device->ConfigurationSpace;
    uint64_t PCIAddress = Device->Address;

    // align buffer
    BackBuffer &= ~(0b11);
    FrontBuffer &= ~(0b11);

    for(size64_t i = 0; i < PCI_CONFIGURATION_SPACE_SIZE; i += 4){ // increment 32 bits
        if(*(uint32_t*)BackBuffer != *(uint32_t*)FrontBuffer){
            PCIWrite32(PCIAddress, *(uint32_t*)FrontBuffer);
        }
        PCIAddress += 4;
        BackBuffer += 4;
        FrontBuffer += 4;
    }

    // update back buffer
    memcpy((void*)BackBuffer, (void*)FrontBuffer, PCI_CONFIGURATION_SPACE_SIZE);
}