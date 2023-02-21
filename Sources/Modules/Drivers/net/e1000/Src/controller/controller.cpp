#include <controller/controller.h>

#include <kot++/printf.h>

static uint64_t E1000Lock;

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

    uint8_t bufferExample[PACKET_SIZE];
    memset(bufferExample, 0, PACKET_SIZE);

    uint32_t ipsrc = 0x54400001;
    uint32_t ipdest = 0x7F000001;

    uint8_t arp_buffer[42];

    // Ethernet header
    memcpy((uintptr_t)&arp_buffer[0], (uintptr_t)"\xff\xff\xff\xff\xff\xff", 6); // Destination MAC address (broadcast)
    memcpy((uintptr_t)&arp_buffer[6], (uintptr_t)MACAddr, 6); // Source MAC address
    memcpy((uintptr_t)&arp_buffer[12], (uintptr_t)"\x08\x06", 2); // Ethernet Type: ARP

    // ARP header
    memcpy((uintptr_t)&arp_buffer[14], (uintptr_t)"\x00\x01", 2); // Hardware Type: Ethernet
    memcpy((uintptr_t)&arp_buffer[16], (uintptr_t)"\x08\x00", 2); // Protocol Type: IPv4
    arp_buffer[18] = 6; // Hardware Address Length: 6 (Ethernet MAC address)
    arp_buffer[19] = 4; // Protocol Address Length: 4 (IPv4 address)
    memcpy((uintptr_t)&arp_buffer[20], (uintptr_t)"\x00\x01", 2); // Operation: ARP Request
    memcpy((uintptr_t)&arp_buffer[22], (uintptr_t)MACAddr, 6); // Sender Hardware Address: Source MAC address
    memcpy((uintptr_t)&arp_buffer[28], (uintptr_t)&ipsrc, 4); // Sender Protocol Address: Source IP address
    memcpy((uintptr_t)&arp_buffer[32], (uintptr_t)"\x00\x00\x00\x00\x00\x00", 6); // Target Hardware Address: zero (unknown)
    memcpy((uintptr_t)&arp_buffer[38], (uintptr_t)&ipdest, 4); // Target Protocol Address: Destination IP address

    memcpy(bufferExample, arp_buffer, 42);

    SendPacket(bufferExample, PACKET_SIZE);
}

void E1000::InitTX() {
    uintptr_t TXDescPhysicalAddr;
    TXDesc = (TXDescriptor*) GetPhysical((uintptr_t*) &TXDescPhysicalAddr, ChipInfo->NumTXDesc * sizeof(TXDescriptor));

    uint32_t PacketBufferSize = ChipInfo->NumTXDesc * PACKET_SIZE;
    TXPacketBuffer = (uint8_t*) GetFreeAlignedSpace(PacketBufferSize);
    Sys_CreateMemoryField(Proc, PacketBufferSize, (uintptr_t*) &TXPacketBuffer, NULL, MemoryFieldTypeShareSpaceRW);

    for(uint8_t i = 0; i < ChipInfo->NumTXDesc; i++) {
        TXDesc[i].BufferAddress = (uint64_t) Sys_GetPhysical(&TXPacketBuffer[i * PACKET_SIZE]);
        TXDesc[i].Length = 0;
        TXDesc[i].Cso = 0;
        TXDesc[i].Cmd = TX_CMD_EOP | TX_CMD_IFCS /* insert FCS (Frame Check Sequence) */ | TX_CMD_RS; 
        TXDesc[i].Status = TX_STATUS_DD;
        TXDesc[i].Css = 0;
        TXDesc[i].Special = 0;
    }

    WriteRegister(TSTD_ADDR_LOW, (uint32_t) ((uint64_t)TXDescPhysicalAddr & 0xFFFFFFFF));
    WriteRegister(TSTD_ADDR_HIGH, (uint32_t) ((uint64_t)TXDescPhysicalAddr >> 32));
    
    WriteRegister(TSTD_LENGTH, ChipInfo->NumTXDesc * sizeof(TXDescriptor));

    // index initialization
    WriteRegister(TSTD_TAIL, 0);
    WriteRegister(TSTD_HEAD, 0);

    // activate transmission
    WriteRegister(REG_TST_CTRL, ReadRegister(REG_TST_CTRL) | TST_EN_MASK | TST_PSP_MASK);
}

void E1000::SendPacket(uint8_t* Data, uint32_t Size) {
    if(Size > PACKET_SIZE)
        return;

    atomicAcquire(&E1000Lock, 0);
    // get the index of the descriptor to use
    uint8_t Index = (uint8_t) ReadRegister(TSTD_TAIL);
    
    while((TXDesc[Index].Status & TX_STATUS_DD) == false){
        asm volatile("":::"memory");
    }

    // copy data to the correct buffer of the correct tx descriptor
    memcpy((uintptr_t)(&TXPacketBuffer[Index * PACKET_SIZE]), (uintptr_t) Data, Size);

    TXDesc[Index].Length = Size;
    TXDesc[Index].Status &= ~TX_STATUS_DD; // undone -> status = 0

    WriteRegister(TSTD_TAIL, (Index + 1) % ChipInfo->NumTXDesc);

    // wait for the packet to be sent
    while((TXDesc[Index].Status & TX_STATUS_DD) == false){
        asm volatile("":::"memory");
    }

    std::printf("Packet Sent TXDescIndex: %d", Index);
    atomicUnlock(&E1000Lock, 0);
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