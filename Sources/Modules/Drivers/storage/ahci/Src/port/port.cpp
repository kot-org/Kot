#include <port/port.h>

Port::Port(AHCIController* Parent, HBAPort_t* Port, PortTypeEnum Type, uint8_t Index){
    Controller = Parent;
    HbaPort = Port;
    PortType = Type;
    PortIndex = Index;

    // Rebase port
    StopCMD();
    CommandHeader = (HBACommandHeader_t*)GetPhysical((uintptr_t*)&HbaPort->CommandListBase, 1024);
    memset((uintptr_t)CommandHeader, 0, 1024);

    uintptr_t FISBaseVirtual = GetPhysical((uintptr_t*)&HbaPort->FisBaseAddress, 256);
    memset(FISBaseVirtual, 0, 256);

    for (int i = 0; i < HBA_COMMAND_LIST_MAX_ENTRIES; i++){
        CommandHeader[i].PrdtLength = 8;
        CommandAddressTable[i] = (HBACommandTable_t*)GetPhysical((uintptr_t*)&CommandHeader[i].CommandTableBaseAddress, 256);
        memset(CommandAddressTable[i], 0, 256);
    }

    StartCMD();

    // allocate buffer
    BufferSize = 0x1000;
    BufferVirtual = GetPhysical((uintptr_t*)&BufferPhysical, BufferSize);

    uint8_t* buffer = (uint8_t*)calloc(0x1000);
    KResult status = Read(0x0, 0x1, buffer);
    for(uint64_t i = 0; i < 0x20; i++){
        std::printf("%x", *buffer++);
    }
    std::printf("%x %x", PortType, status);
}

Port::~Port(){

}

void Port::StopCMD(){
    HbaPort->CommandStatus &= ~HBA_PxCMD_ST;
    HbaPort->CommandStatus &= ~HBA_PxCMD_FRE;

    while(true){
        if (HbaPort->CommandStatus & HBA_PxCMD_FR) continue;
        if (HbaPort->CommandStatus & HBA_PxCMD_CR) continue;

        break;
    }

}

void Port::StartCMD(){
    while (HbaPort->CommandStatus & HBA_PxCMD_CR);

    HbaPort->CommandStatus |= HBA_PxCMD_FRE;
    HbaPort->CommandStatus |= HBA_PxCMD_ST;
}

KResult Port::Read(uint64_t Sector, uint16_t SectorCount, uintptr_t Buffer){
    memset(BufferVirtual, 0x0, SectorCount * 512);
    uint32_t SectorLow = (uint32_t)Sector & 0xFFFFFFFF;
    uint32_t sectorHigh = (uint32_t)(Sector >> 32) & 0xFFFFFFFF;

    HbaPort->InterruptStatus = (uint32_t)-1; // Clear pending interrupt bits

    CommandHeader->CommandFISLength = sizeof(FisHostToDeviceRegisters_t) / sizeof(uint32_t); //command FIS size;
    CommandHeader->Atapi = 0;
    CommandHeader->Write = 0; //read mode
    CommandHeader->PrdtLength = 1;

    HBACommandTable_t* CommandTable = CommandAddressTable[0];
    memset(CommandTable, 0, sizeof(HBACommandTable_t) + (CommandHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry_t));

    CommandTable->PrdtEntry[0].DataBaseAddress = (uint64_t)BufferPhysical;
    CommandTable->PrdtEntry[0].ByteCount = (SectorCount << 9) - 1; // 512 bytes per sector

    FisHostToDeviceRegisters_t* CommandFIS = (FisHostToDeviceRegisters_t*)(&CommandTable->CommandFIS);

    CommandFIS->FisType = FISTypeEnum::HostToDevice;
    CommandFIS->CommandControl = 1; // command
    CommandFIS->Command = ATACommandEnum::ReadDMA; //read command

    CommandFIS->Lba0 = (uint8_t)SectorLow;
    CommandFIS->Lba1 = (uint8_t)(SectorLow >> 8);
    CommandFIS->Lba2 = (uint8_t)(SectorLow >> 16);
    CommandFIS->Lba3 = (uint8_t)sectorHigh;
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 8);
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 16);

    CommandFIS->DeviceRegister = 1 << 6; //LBA mode

    CommandFIS->CountLow = SectorCount & 0xFF;
    CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

    uint64_t spin = 0;
    while((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        return KFAIL;
    }

    HbaPort->CommandIssue = 1;

    while (true){
        if(HbaPort->CommandIssue == 0) break;
        if(HbaPort->InterruptStatus & HBA_INTERRUPT_STATU_TFE){
            return KFAIL;
        }
    }

    memcpy(Buffer, BufferVirtual, SectorCount << 9); // copy data to DMA buffer
    return KSUCCESS;
}