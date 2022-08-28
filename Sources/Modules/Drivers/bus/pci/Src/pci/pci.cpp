#include <pci/pci.h>

/* PRIVATE */

PCIBar* PCIGetBaseAddressRegister(uint32_t deviceAddr, uint8_t barID, PCIHeader0* header) {
    if(header->BAR[barID] != NULL){
        PCIBar* BaseAddrReg = (PCIBar*)malloc(sizeof(PCIBar));
        uint64_t barSizeLow = 0;
        uint64_t barSizeHigh = 0xFFFFFFFF;

        if(header->BAR[barID] & 0b1){ /* i/o */
            BaseAddrReg->Type = 0x1;
            BaseAddrReg->Base = (header->BAR[barID] & 0xFFFFFFFC);
        }else{
            if(!(header->BAR[barID] & 0b110)){ /* 32bits */
                BaseAddrReg->Type = 0x2;
                BaseAddrReg->Base = (header->BAR[barID] & 0xFFFFFFF0);
            }else if((header->BAR[barID] & 0b110) == 0b110){ /* 64bits */
                BaseAddrReg->Type = 0x3;
                BaseAddrReg->Base = ((header->BAR[barID] & 0xFFFFFFF0) | ((header->BAR[barID + 1] & 0xFFFFFFFF) << 32));
            }
        }


        /* Size low */
        PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET + barID * 0x4, 0xFFFFFFFF);

        barSizeLow = PCIRead32(deviceAddr + PCIH0_BAR0_OFFSET + barID * 0x4);

        if(BaseAddrReg->Type == 0x1){
            barSizeLow &= 0xFFFFFFFC;
        }else{
            barSizeLow &= 0xFFFFFFF0;
        }

        PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET + barID * 0x4, header->BAR[barID]);

        /* Size high */
        if(BaseAddrReg->Type == 0x3) {
            PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET + (barID + 1) * 0x4, 0xFFFFFFFF);

            barSizeHigh = PCIRead32(deviceAddr + PCIH0_BAR0_OFFSET + (barID + 1) * 0x4);
            barSizeHigh &= 0xFFFFFFFF;

            PCIWrite32(deviceAddr + PCIH0_BAR0_OFFSET + (barID + 1) * 0x4, header->BAR[barID + 1]);
        }

        uint64_t barSize = barSizeLow | (barSizeHigh << 32);
        BaseAddrReg->Size = ~barSize + 1;
        return BaseAddrReg;
    }
    return NULL;
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

            self = (PCIDevice*)calloc(sizeof(PCIDevice));

            self->Header = (PCIDeviceHeader*)Header;
            for(uint8_t i = 0; i <= 5; i++) {
                PCIBar* BAR = PCIGetBaseAddressRegister(Addr, i, (PCIHeader0*)Header);
                if(BAR != NULL){
                    if(BAR->Type != 0x0) {
                        self->BAR[self->BARNum] = BAR;
                        self->BARNum++;

                        if(BAR->Type == 0x3) i++;
                    }
                }
            }
            self->Index = PCIDevicesIndex;
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
    uint64_t PCIDevicesIndexTmp = 1;
    PCIDevicesIndex = 1;
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

uint64_t PCISearcher(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIf) {
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

    for(uint32_t i = 1; i < PCIDevicesIndex; i++) {

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

PCIDeviceID_t PCISearcherGetDevice(uint16_t vendorID, uint16_t deviceID, uint16_t subClassID, uint16_t classID, uint16_t progIf, uint64_t index) {
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

    for(uint32_t i = 1; i < PCIDevicesIndex; i++) {
        
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

        if(index == deviceNum){
            return i;
        }

    }
    return NULL;
}