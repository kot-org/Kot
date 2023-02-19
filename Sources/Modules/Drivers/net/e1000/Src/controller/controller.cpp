#include <controller/controller.h>

#include <kot++/printf.h>

void E1000::WriteRegister(uint16_t Reg, uint32_t Value) {
    if(BarType == PCI_BAR_TYPE_IO) {
        IoWrite32(IoBase, Reg);
        IoWrite32(IoBase + sizeof(uint32_t), Value);
    } else {
        MmioWrite32((uintptr_t) ((uint64_t)MemoryBase + Reg), Value);
    }
}

uint32_t E1000::ReadRegister(uint16_t Reg) {
    if(BarType == PCI_BAR_TYPE_IO) {
        IoWrite32(IoBase, Reg);
        return IoRead32(IoBase + sizeof(uint32_t));
    } else {
        return MmioRead32((uintptr_t) ((uint64_t)MemoryBase + Reg));
    }
}

E1000::E1000(srv_pci_device_info_t* DeviceInfo, srv_pci_bar_info_t* BarInfo) {
    BarType = BarInfo->Type;

    if(BarType == PCI_BAR_TYPE_IO)
        IoBase = (uint64_t) BarInfo->Address;
    else
        MemoryBase = MapPhysical(BarInfo->Address, BarInfo->Size);

    ChipInfo = GetChipInfo(DeviceInfo->deviceID);

    // reset the device
    WriteRegister(REG_CTRL, ReadRegister(REG_CTRL) | CTRL_RESET_MASK);

    // reconfigure
    WriteRegister(REG_CTRL, ReadRegister(REG_CTRL) | CTRL_ASDE_MASK | CTRL_SLU_MASK);

    // get mac address
    /* LOW */
    MACAddr[0] = (uint8_t) ReadRegister(RCV_ADDR_LOW) & 0xFF;
    MACAddr[1] = (uint8_t) (ReadRegister(RCV_ADDR_LOW) >> 8) & 0xFF;
    MACAddr[2] = (uint8_t) (ReadRegister(RCV_ADDR_LOW) >> 16) & 0xFF;
    MACAddr[3] = (uint8_t) (ReadRegister(RCV_ADDR_LOW) >> 24) & 0xFF;
    /* HIGH */
    MACAddr[4] = (uint8_t) ReadRegister(RCV_ADDR_HIGH) & 0xFF;
    MACAddr[5] = (uint8_t) (ReadRegister(RCV_ADDR_HIGH) >> 8) & 0xFF;

    // set bit "Address valid"
    WriteRegister(RCV_ADDR_HIGH, ReadRegister(RCV_ADDR_HIGH) | RAH_AV_MASK);
    
    std::printf("[NET/E1000] \n DeviceID: 0x%x\n BarType: %d\n Speed: %dMb/s\n MAC address: %x:%x:%x:%x:%x:%x", DeviceInfo->deviceID, BarType, GetSpeed(), MACAddr[0], MACAddr[1], MACAddr[2], MACAddr[3], MACAddr[4], MACAddr[5]);

    InitTX();
}

void E1000::InitTX() {
    TXDescriptor* TXDesc = (TXDescriptor*) malloc(ChipInfo->NumTXDesc * sizeof(TXDescriptor));

    size_t PacketBufferSize = ChipInfo->NumTXDesc * PACKET_SIZE;
    uint8_t* PacketBuffer = (uint8_t*) GetFreeAlignedSpace(PacketBufferSize);
    Sys_CreateMemoryField(Proc, PacketBufferSize, (uintptr_t*) &PacketBuffer, NULL, MemoryFieldTypeShareSpaceRW);

    for(uint8_t i = 0; i < ChipInfo->NumTXDesc; i++) {
        TXDesc[i].BufferAddress = (uint64_t) Sys_GetPhysical(&PacketBuffer[i * PACKET_SIZE]);
        TXDesc[i].Length = 0;
        TXDesc[i].Cmd = TX_CMD_EOP | TX_CMD_IFCS; // insert FCS (Frame Check Sequence) 
        TXDesc[i].Status = TX_STATUS_DD;
    }

    
}

uint16_t E1000::GetSpeed() {
    switch((ReadRegister(REG_STATUS) & STATUS_SPEED_MASK) >> 6)
    {
        case 0x0:
            return 10;
        case 0x1:
            return 100;
        case 0x2:
            return 1000;

        default:
            return 0;
    }
}

ChipInfo_t* GetChipInfo(uint16_t DeviceID) {
    ChipInfo_t* Chip = (ChipInfo_t*) malloc(sizeof(ChipInfo_t));

    Chip->DeviceID = DeviceID;

    switch(DeviceID)
    {
        case E1000_DEV:
            Chip->NumRXDesc = 64;
            Chip->NumTXDesc = 64;
            break;
        
        case E1000_I217:
            break;

        // todo

        default:
            Printlog("[NET/E1000] This DeviceID does not exist. (exit)");
            Sys_Close(KFAIL);
            break;
    }

    return Chip;
}