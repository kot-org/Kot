#include <core/main.h>

#include <tools/config.h>
#include <tools/memory.h>

PCIBar* PCIGetBaseAddressRegister(uint32_t deviceAddr) {
    PCIBar* BaseAddrReg = (PCIBar*) malloc(sizeof(PCIBar));
    uint32_t bar = PCIRead32(deviceAddr + PCIH0_BAR0_OFFSET);

    if((bar & 0b0111) == 0b0110) { /* 64bits */
        BaseAddrReg->Type = 0x3;
        BaseAddrReg->Base = (bar & 0xFFFFFFF0);
    } else if((bar & 0b0111) == 0b0001) { /* I/O */
        BaseAddrReg->Type = 0x1;
        BaseAddrReg->Base = (bar & 0xFFFFFFFC);
    } else { /* 32bits */
        BaseAddrReg->Type = 0x2;
        BaseAddrReg->Base = (bar & 0xFFFFFFF0);
    }

    PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET, 1);

    BaseAddrReg->Size = PCIRead32(deviceAddr + PCIH0_BAR0_OFFSET);

    PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET, bar);

    char buffer[100], buffernum[20];
    *buffer = NULL;

    strcat(buffer, "[PCI] Size: 0x");
    itoa(BaseAddrReg->Size, buffernum, 16);
    strcat(buffer, buffernum);

    Printlog(buffer);

    return BaseAddrReg;
}

uint32_t PCIDeviceBaseAddress(uint16_t bus, uint16_t device, uint16_t func){
    /**
     *   -------------------------------------------------------------------------------------------------------------
     *   |  Bit 31	    |   Bits 30-24	|  Bits 23-16	|  Bits 15-11	  |   Bits 10-8	        |   Bits 7-0         |
     *   |  Enable Bit	|   Reserved	|  Bus Number	|  Device Number  |   Function Number	|   Register Offset  |
     *   -------------------------------------------------------------------------------------------------------------
     */
    return (uint32_t) ((1 <<  31) | (bus << 16) | (device << 11) | (func << 8));
}

uintptr_t GetDevice(uint16_t bus, uint16_t device, uint16_t func){
    uint32_t Addr = PCIDeviceBaseAddress(bus, device, func);
    uint16_t VendorID = PCIRead16(Addr + PCI_VENDOR_ID_OFFSET);

    if(VendorID == 0xffff) return NULL;

    uint8_t HeaderType = PCIRead8(Addr + PCI_HEADER_TYPE_OFFSET);
    HeaderType &= ~(1 << 7);
    uintptr_t Header = NULL;
    PCIBar* BaseAddrReg;
    
    switch (HeaderType){
        case 0x0:
            Header = malloc(sizeof(PCIHeader0));
            PCIMemcpyToMemory32(Header, Addr, sizeof(PCIHeader0));
            char buffer[100], buffernum[33];
            *buffer = NULL;

            BaseAddrReg = PCIGetBaseAddressRegister(Addr);

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

            strcat(buffer, " Bar0: 0x");
            itoa(((PCIHeader0*)Header)->BAR[0], buffernum, 16);
            strcat(buffer, buffernum);

            Printlog(buffer);
            break;
        case 0x1:
            /* TODO */
            Printlog("[Error] PCI-to-PCI bridge not supported");
            break;
        default:
            Printlog("[Error] Unknow header type");
            return 0;
            break;
    }
    return Header;
}

void EnumerateDevices() {
    for(uint32_t bus = 0; bus < 256; bus++) {
        for(uint32_t device = 0; device < 32; device++) {

            uint32_t Addr = PCIDeviceBaseAddress(bus, device, NULL);
            uint16_t vendorID = PCIRead16(Addr + PCI_VENDOR_ID_OFFSET);
            
            if(vendorID == 0xffff) continue;

            uint8_t headerType = PCIRead8(Addr + PCI_HEADER_TYPE_OFFSET);

            if((headerType & 0x80) != 0){
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
