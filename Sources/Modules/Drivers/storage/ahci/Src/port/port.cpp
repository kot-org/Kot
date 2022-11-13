#include <port/port.h>

Device::Device(AHCIController* Parent, HBAPort_t* Port, PortTypeEnum Type, uint8_t Index){
    Controller = Parent;
    HbaPort = Port;
    PortType = Type;
    PortIndex = Index;

    // Rebase port
    StopCMD();
    CommandHeader = (HBACommandHeader_t*)GetPhysical((uintptr_t*)&HbaPort->CommandListBase, 1024);
    memset((uintptr_t)CommandHeader, NULL, 1024);

    uintptr_t FISBaseVirtual = GetPhysical((uintptr_t*)&HbaPort->FisBaseAddress, 256);
    memset(FISBaseVirtual, NULL, 256);

    BufferRealSize = HBA_PRDT_MAX_ENTRIES * HBA_PRDT_ENTRY_ADDRESS_SIZE;
    BufferAlignement = ATA_SECTOR_SIZE;
    BufferUsableSize = BufferRealSize - BufferAlignement; // Remove one sector for alignement

    BufferAlignementBottom = malloc(BufferAlignement);

    MainSlot = FindSlot();
    if(MainSlot == -1){
        return;
    }

    DefaultSpace = CreateSpace(NULL, NULL);
    LoadSpace(DefaultSpace);
    StartCMD();

    // Identify disk
    IdentifyInfo = (IdentifyInfo_t*)calloc(sizeof(IdentifyInfo_t));
    GetIdentifyInfo(DefaultSpace);

    // Update space size
    DefaultSpace->Size = GetSize();


    SrvAddDevice(this);
}

Device::~Device(){

}

void Device::StopCMD(){
    HbaPort->CommandStatus &= ~HBA_PxCMD_ST;
    HbaPort->CommandStatus &= ~HBA_PxCMD_FRE;

    while(true){
        if(HbaPort->CommandStatus & HBA_PxCMD_FR) continue;
        if(HbaPort->CommandStatus & HBA_PxCMD_CR) continue;

        break;
    }

}

void Device::StartCMD(){
    while(HbaPort->CommandStatus & HBA_PxCMD_CR);

    HbaPort->CommandStatus |= HBA_PxCMD_FRE;
    HbaPort->CommandStatus |= HBA_PxCMD_ST;
}

int8_t Device::FindSlot(){
    uint32_t Slot = HbaPort->SataActive;
    for(uint8_t i = 0; i < HBA_COMMAND_LIST_MAX_ENTRIES; i++){
        if((Slot & 1) == 0){
            return i;
        }
        Slot >>= 1;
    }
    return -1;
}

Space_t* Device::CreateSpace(uint64_t Start, uint64_t Size){
    Space_t* Self = (Space_t*)malloc(sizeof(Space_t));
    Self->Start = Start;
    Self->Size = Size;
    Self->StorageDevice = this;

    // Allocate buffer
    Self->BufferVirtual = GetFreeAlignedSpace(BufferRealSize);
    Sys_CreateMemoryField(Proc, BufferRealSize, &Self->BufferVirtual, &Self->BufferKey, MemoryFieldTypeShareSpaceRW);

    // Load command header main
    Self->CommandAddressTable = (HBACommandTable_t*)GetPhysical((uintptr_t*)&CommandHeader[0].CommandTableBaseAddress, HBA_COMMAND_TABLE_SIZE);

    uint64_t BufferInteration = (uint64_t)Self->BufferVirtual;
    for(size64_t i = 0; i < HBA_PRDT_MAX_ENTRIES; i++){
        Self->CommandAddressTable->PrdtEntry[i].DataBaseAddress = (uint64_t)Sys_GetPhysical((uintptr_t)BufferInteration);
        BufferInteration = (uint64_t)BufferInteration + HBA_PRDT_ENTRY_ADDRESS_SIZE;
    }

    return Self;
}

void Device::LoadSpace(Space_t* Self){
    // Load command header main
    CommandAddressTable[MainSlot] = Self->CommandAddressTable;
}

KResult Device::Read(Space_t* Self, uint64_t Start, size64_t Size){
    uint64_t StartAlignement = Start & 0x1FF;
    uint64_t Sector = Start >> 9;
    uint64_t SectorCount = DivideRoundUp(Size + StartAlignement, ATA_SECTOR_SIZE);
    uint64_t PRDTLength = DivideRoundUp(SectorCount, HBA_PRDT_ENTRY_SECTOR_SIZE);

    if(PRDTLength > HBA_PRDT_MAX_ENTRIES){
        return KFAIL;
    }

    uint32_t SectorLow = (uint32_t)Sector & 0xFFFFFFFF;
    uint32_t sectorHigh = (uint32_t)(Sector >> 32) & 0xFFFFFFFF;

    HbaPort->InterruptStatus = (uint32_t)-1; // Clear pending interrupt bits

    CommandHeader->CommandFISLength = sizeof(FisHostToDeviceRegisters_t) / sizeof(uint32_t); // Command FIS size;
    CommandHeader->Atapi = 0;
    CommandHeader->Write = 0; // Read mode
    CommandHeader->PrdtLength = PRDTLength;

    HBACommandTable_t* CommandTable = CommandAddressTable[MainSlot];
    FisHostToDeviceRegisters_t* CommandFIS = (FisHostToDeviceRegisters_t*)(&CommandTable->CommandFIS);

    // Load PRDT

    uint64_t SectorCountIteration = SectorCount;

    for(uint16_t i = 0; i < PRDTLength; i++){
        uint64_t SectorCountToLoad = SectorCountIteration;
        if(SectorCountToLoad > HBA_PRDT_ENTRY_SECTOR_SIZE){
            SectorCountToLoad = HBA_PRDT_ENTRY_SECTOR_SIZE;
        }

        CommandTable->PrdtEntry[i].ByteCount = (SectorCountToLoad << 9) - 1; // 512 bytes per sector
        CommandTable->PrdtEntry[i].InterruptOnCompletion = 1; 
        SectorCountIteration -= SectorCountToLoad;
	}

    CommandFIS->FisType = FISTypeEnum::HostToDevice;
    CommandFIS->CommandControl = 1; // Command
    CommandFIS->Command = ATACommandEnum::ReadDMA; // Read command

    CommandFIS->Lba0 = (uint8_t)SectorLow & 0xFF;
    CommandFIS->Lba1 = (uint8_t)(SectorLow >> 8) & 0xFF;
    CommandFIS->Lba2 = (uint8_t)(SectorLow >> 16) & 0xFF;
    CommandFIS->Lba3 = (uint8_t)sectorHigh & 0xFF;
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 8) & 0xFF;
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 16) & 0xFF;

    CommandFIS->DeviceRegister = 1 << 6; // LBA mode

    CommandFIS->CountLow = SectorCount & 0xFF;
    CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

    uint64_t spin = 0;
    while((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        return KFAIL;
    }

    HbaPort->CommandIssue = 1 << MainSlot;

    while (true){
        if((HbaPort->CommandIssue & (1 << MainSlot)) == 0) break;
        if(HbaPort->InterruptStatus & HBA_INTERRUPT_STATU_TFE){
            return KFAIL;
        }
    }

    return KSUCCESS;
}

KResult Device::Write(Space_t* Self, uint64_t Start, size64_t Size){
    uint64_t StartAlignement = Start & 0x1FF;
    uint64_t StartAlignementFill = ATA_SECTOR_SIZE - StartAlignement;

    uint64_t EndAlignement = (Start + Size) & 0x1FF;
    uint64_t EndAlignementFill = ATA_SECTOR_SIZE - EndAlignement;

    uint64_t BufferSizeUsed = Size + StartAlignement;
    
    uint64_t Sector = Start >> 9;
    uint64_t SectorCount = DivideRoundUp(BufferSizeUsed, ATA_SECTOR_SIZE);
    uint64_t PRDTLength = DivideRoundUp(SectorCount, HBA_PRDT_ENTRY_SECTOR_SIZE);

    if(PRDTLength > HBA_PRDT_MAX_ENTRIES){
        return KFAIL;
    }

    if((StartAlignement && SectorCount == 0x1) || (EndAlignement && SectorCount == 0x1)){
        uintptr_t BufferTmp = (uintptr_t)((uint64_t)Self->BufferVirtual + StartAlignement);
        memcpy(BufferAlignementBottom, BufferTmp, Size);
        Read(Self, Start, ATA_SECTOR_SIZE);
        memcpy(BufferTmp, BufferAlignementBottom, Size);
    }else{
        if(StartAlignement){
            uintptr_t BufferTmp = (uintptr_t)((uint64_t)Self->BufferVirtual + StartAlignement);
            memcpy(BufferAlignementBottom, BufferTmp, StartAlignementFill);
            Read(Self, Start, ATA_SECTOR_SIZE);
            memcpy(BufferTmp, BufferAlignementBottom, StartAlignementFill);
        }else if(EndAlignement){
            memcpy(BufferAlignementBottom, Self->BufferVirtual, ATA_SECTOR_SIZE);
            uint64_t EndLocation = Start + Size - EndAlignement;
            uintptr_t BufferTmp = (uintptr_t)((uint64_t)Self->BufferVirtual + EndAlignement);
            uintptr_t BufferDst = (uintptr_t)((uint64_t)Self->BufferVirtual + Start + Size);
            Read(Self, EndLocation, ATA_SECTOR_SIZE);
            memcpy(BufferDst, BufferTmp, EndAlignementFill);
            memcpy(Self->BufferVirtual, BufferAlignementBottom, ATA_SECTOR_SIZE);
        }
    }
    
    LoadSpace(Self);
    uint32_t SectorLow = (uint32_t)Sector & 0xFFFFFFFF;
    uint32_t sectorHigh = (uint32_t)(Sector >> 32) & 0xFFFFFFFF;

    HbaPort->InterruptStatus = (uint32_t)-1; // Clear pending interrupt bits

    CommandHeader->CommandFISLength = sizeof(FisHostToDeviceRegisters_t) / sizeof(uint32_t); // Command FIS size;
    CommandHeader->Atapi = 0;
    CommandHeader->Write = 1; // Read mode
    CommandHeader->PrdtLength = PRDTLength;

    HBACommandTable_t* CommandTable = CommandAddressTable[MainSlot];
    FisHostToDeviceRegisters_t* CommandFIS = (FisHostToDeviceRegisters_t*)(&CommandTable->CommandFIS);

    // Load PRDT
    uint64_t SectorCountIteration = SectorCount;

    for(uint16_t i = 0; i < CommandHeader->PrdtLength; i++){
        uint64_t SectorCountToLoad = SectorCountIteration;
        if(SectorCountToLoad > HBA_PRDT_ENTRY_SECTOR_SIZE){
            SectorCountToLoad = HBA_PRDT_ENTRY_SECTOR_SIZE;
        }

        CommandTable->PrdtEntry[i].ByteCount = (SectorCountToLoad << 9) - 1; // 512 bytes per sector
        CommandTable->PrdtEntry[i].InterruptOnCompletion = 1; 

        SectorCountIteration -= SectorCountToLoad;
	}

    CommandFIS->FisType = FISTypeEnum::HostToDevice;
    CommandFIS->CommandControl = 1; // Command
    CommandFIS->Command = ATACommandEnum::WriteDMA; // Read command

    CommandFIS->Lba0 = (uint8_t)SectorLow & 0xFF;
    CommandFIS->Lba1 = (uint8_t)(SectorLow >> 8) & 0xFF;
    CommandFIS->Lba2 = (uint8_t)(SectorLow >> 16) & 0xFF;
    CommandFIS->Lba3 = (uint8_t)sectorHigh & 0xFF;
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 8) & 0xFF;
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 16) & 0xFF;

    CommandFIS->DeviceRegister = 1 << 6; // LBA mode

    CommandFIS->CountLow = SectorCount & 0xFF;
    CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

    uint64_t spin = 0;
    while((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        return KFAIL;
    }

    HbaPort->CommandIssue = 1 << MainSlot;

    while (true){
        if((HbaPort->CommandIssue & (1 << MainSlot)) == 0) break;
        if(HbaPort->InterruptStatus & HBA_INTERRUPT_STATU_TFE){
            return KFAIL;
        }
    }

    return KSUCCESS;
}

KResult Device::GetIdentifyInfo(Space_t* Self){
    LoadSpace(Self);
    HbaPort->InterruptStatus = NULL; // Clear pending interrupt bits

    CommandHeader->CommandFISLength = sizeof(FisHostToDeviceRegisters_t) / sizeof(uint32_t); // Command FIS size;
    CommandHeader->Atapi = 0;
    CommandHeader->Write = 0; // Read mode
    CommandHeader->PrdtLength = 1;

    HBACommandTable_t* CommandTable = CommandAddressTable[MainSlot];
    memset(CommandTable, 0, sizeof(HBACommandTable_t) + (CommandHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry_t));

    CommandTable->PrdtEntry[0].ByteCount = sizeof(IdentifyInfo_t) - 1; // 512 bytes per sector
    CommandTable->PrdtEntry[0].InterruptOnCompletion = 1; 

    FisHostToDeviceRegisters_t* CommandFIS = (FisHostToDeviceRegisters_t*)(&CommandTable->CommandFIS);

    CommandFIS->FisType = FISTypeEnum::HostToDevice;
    CommandFIS->CommandControl = 1; // Command
    CommandFIS->Command = ATACommandEnum::Identify; // Identify command

    uint64_t spin = 0;
    while((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        return KFAIL;
    }

    HbaPort->CommandIssue = 1 << MainSlot;

    while (true){
        if((HbaPort->CommandIssue & (1 << MainSlot)) == 0) break;
        if(HbaPort->InterruptStatus & HBA_INTERRUPT_STATU_TFE){
            return KFAIL;
        }
    }
    memcpy(IdentifyInfo, Self->BufferVirtual, sizeof(IdentifyInfo_t)); // copy data to DMA buffer

    return KSUCCESS; 
}

uint64_t Device::GetSize(){
    uint64_t Size = NULL;
    if(IdentifyInfo->ExtendedNumberOfUserAddressableSectors){
        Size = (uint64_t)IdentifyInfo->ExtendedNumberOfUserAddressableSectors << 9;
    }else{
        Size = (uint64_t)IdentifyInfo->TotalUserAddressableSectors << 9;
    }
    return Size;
}

uint8_t* Device::GetModelNumber(){
    return IdentifyInfo->DriveModelNumber;
}

uint8_t* Device::GetSerialNumber(){
    return IdentifyInfo->SerialNumber;
}