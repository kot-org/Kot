#include <core/main.h>

#include <tools/config.h>
#include <tools/memory.h>

PCIDevice** PCIDevices = NULL;
uint32_t PCIDevicesIndex = 0;

/* PRIVATE */

PCIBar* PCIGetBaseAddressRegister(uint32_t deviceAddr, uint8_t barID, PCIHeader0* header) {
    PCIBar* BaseAddrReg = (PCIBar*)malloc(sizeof(PCIBar));
    uint32_t barSizeLow = 0, barSizeHigh = ~0;

    if(header->BAR[barID] & 0b1){ /* i/o */
        BaseAddrReg->Type = 0x1;
        BaseAddrReg->Base = (header->BAR[barID] & 0xFFFFFFFC);
    }else{
        if(!(header->BAR[barID] & 0b110)){ /* 32bits */
            BaseAddrReg->Type = 0x2;
            BaseAddrReg->Base = (header->BAR[barID] & 0xFFFFFFF0);
        }else if((header->BAR[barID] & 0b110) == 0b110){ /* 64bits */
            BaseAddrReg->Type = 0x3;
            BaseAddrReg->Base = ((header->BAR[barID] & 0xFFFFFFF0) + ((header->BAR[barID + 1] & 0xFFFFFFFF) << 32));
        }
    }

    /* Size low */
    PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET + barID * 0x4, ~0);

    barSizeLow = PCIRead32(deviceAddr + PCIH0_BAR0_OFFSET + barID * 0x4);

    PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET + barID * 0x4, header->BAR[barID]);

    /* Size high */
    if(BaseAddrReg->Type == 0x3) {
        PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET + (barID + 1) * 0x4, ~0);

        barSizeHigh = PCIRead32(deviceAddr + PCIH0_BAR0_OFFSET + (barID + 1) * 0x4);

        PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET + (barID + 1) * 0x4, header->BAR[barID + 1]);
    }

    BaseAddrReg->Size = ~((barSizeHigh << 32 | barSizeLow) & ~((BaseAddrReg->Type == 0x1) ? (0b1111) : (0b11))) + 1;

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

bool CheckDevice(uint32_t Addr){
    uint16_t VendorID = PCIRead16(Addr + PCI_VENDOR_ID_OFFSET);
    if(VendorID == 0xffff) return false;
    return true;
}
bool CheckDevice(uint16_t bus, uint16_t device, uint16_t func){
    uint32_t Addr = PCIDeviceBaseAddress(bus, device, func);
    return CheckDevice(Addr);
}

PCIDevice* GetDevice(uint16_t bus, uint16_t device, uint16_t func){
    uint32_t Addr = PCIDeviceBaseAddress(bus, device, func);
    if(!CheckDevice(Addr)) return NULL;

    uint8_t HeaderType = PCIRead8(Addr + PCI_HEADER_TYPE_OFFSET);
    HeaderType &= ~(1 << 7);
    uintptr_t Header = NULL;
    PCIDevice* self;
    PCIBar* BaseAddrReg;
    
    switch (HeaderType){
        case 0x0:
            Header = malloc(sizeof(PCIHeader0));
            PCIMemcpyToMemory32(Header, Addr, sizeof(PCIHeader0));

            char buffer[100], buffernum[33];
            *buffer = NULL;
            BaseAddrReg = PCIGetBaseAddressRegister(Addr, 0, (PCIHeader0*)Header);
            self = (PCIDevice*) calloc(sizeof(PCIDevice));

            self->Header = (PCIDeviceHeader*)Header;
            for(uint8_t i = 0; i <= 5; i++) {
                PCIBar* BAR = PCIGetBaseAddressRegister(Addr, i, (PCIHeader0*)Header);

                if(BAR->Type != 0x0) {
                    self->BAR[self->BARNum] = BAR;
                    self->BARNum++;

                    if(BAR->Type == 0x3) i++;
                }
            }
            self->Index = PCIDevicesIndex;

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
    return self;
}

void EnumerateDevices() {
    uint64_t PCIDevicesIndexTmp = 0;
    for(uint32_t bus = 0; bus < 256; bus++) {
        for(uint32_t device = 0; device < 32; device++) {

            uint32_t Addr = PCIDeviceBaseAddress(bus, device, NULL);
            uint16_t vendorID = PCIRead16(Addr + PCI_VENDOR_ID_OFFSET);
            
            if(vendorID == 0xffff) continue;

            uint8_t headerType = PCIRead8(Addr + PCI_HEADER_TYPE_OFFSET);

            if((headerType & 0x80) != 0){
                for(uint32_t func = 0; func < 8; func++) {
                    if(CheckDevice(bus, device, func)) PCIDevicesIndexTmp++;
                }
            }else{
                if(CheckDevice(bus, device, NULL)) PCIDevicesIndexTmp++;
            }
        }
    }  

    PCIDevices = (PCIDevice**)malloc(sizeof(PCIDevice) * PCIDevicesIndexTmp);

    for(uint32_t bus = 0; bus < 256; bus++) {
        for(uint32_t device = 0; device < 32; device++) {

            uint32_t Addr = PCIDeviceBaseAddress(bus, device, NULL);
            uint16_t vendorID = PCIRead16(Addr + PCI_VENDOR_ID_OFFSET);
            
            if(vendorID == 0xffff) continue;

            uint8_t headerType = PCIRead8(Addr + PCI_HEADER_TYPE_OFFSET);

            if((headerType & 0x80) != 0){
                for(uint32_t func = 0; func < 8; func++) {
                    PCIDevices[PCIDevicesIndex] = GetDevice(bus, device, func);
                    PCIDevicesIndex++;
                }
            }else{
                PCIDevices[PCIDevicesIndex] = GetDevice(bus, device, NULL);
                PCIDevicesIndex++;
            }
        }
    }    
}

extern "C" int main(int argc, char* argv[]) {
    Printlog("[PCI] Initialization ...");

    EnumerateDevices();

    Printlog("[PCI] Driver initialized successfully");

    return KSUCCESS;
}

/* PUBLIC */

bool checkDeviceIndex(uint32_t index) {
    if(index < PCIDevicesIndex)
        return true;
    return false;
}

uint8_t GetBARNum(uint32_t index) {
    if(checkDeviceIndex(index))
        return PCIDevices[index]->BARNum;
    return NULL;
}
uint64_t GetBARSize(uint32_t index, uint8_t barIndex) {
    if(checkDeviceIndex(index) && barIndex < PCIDevices[index]->BARNum)
        return PCIDevices[index]->BAR[barIndex]->Size;
    return NULL;
}

uint32_t PCISearcherGetDevice(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIf, uint64_t index) {
    uint8_t checkRequired = 0;
    uint32_t deviceNum = 0;

    if(vendorID != 0xFFFF)
        checkRequired++;
    if(deviceID != 0xFFFF)
        checkRequired++;
    if(subClassID != 0xFFFF)
        checkRequired++;
    if(classID != 0xFFFF)
        checkRequired++;
    if(progIf != 0xFFFF)
        checkRequired++;

    for(uint32_t i = 0; i < PCIDevicesIndex; i++) {
        
        PCIDeviceHeader header = ((PCIHeader0*)PCIDevices[i])->Header;

        uint8_t checkNum = 0;
        
        if(header.VendorID == vendorID)
            checkNum++;
        if(header.DeviceID == deviceID)
            checkNum++;
        if(header.Subclass == subClassID)
            checkNum++;
        if(header.Class == classID)
            checkNum++;
        if(header.ProgIF == progIf)
            checkNum++;

        if(checkRequired == checkNum) deviceNum++;

        if(index == deviceNum)
            return i;

    }
    return NULL;
}

uint32_t PCISearcher(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIf) {
    uint8_t checkRequired = 0;
    uint32_t deviceNum = 0;

    if(vendorID != 0xFFFF)
        checkRequired++;
    if(deviceID != 0xFFFF)
        checkRequired++;
    if(subClassID != 0xFFFF)
        checkRequired++;
    if(classID != 0xFFFF)
        checkRequired++;
    if(progIf != 0xFFFF)
        checkRequired++;

    for(uint32_t i = 0; i < PCIDevicesIndex; i++) {

        PCIDeviceHeader header = ((PCIHeader0*)PCIDevices[i])->Header;

        uint8_t checkNum = 0;
        
        if(header.VendorID == vendorID)
            checkNum++;
        if(header.DeviceID == deviceID)
            checkNum++;
        if(header.Subclass == subClassID)
            checkNum++;
        if(header.Class == classID)
            checkNum++;
        if(header.ProgIF == progIf)
            checkNum++;

        if(checkRequired == checkNum) deviceNum++;

    }
    return deviceNum;
}
