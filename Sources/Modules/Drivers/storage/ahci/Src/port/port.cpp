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

    // Allocate buffer
    BufferSize = 0x1000;
    BufferVirtual = GetPhysical((uintptr_t*)&BufferPhysical, BufferSize);

    // Be sur to unlock the locker
    atomicUnlock(&Lock, 0);

    // Identify disk
    IdentifyInfo = (IdentifyInfo_t*)calloc(sizeof(IdentifyInfo_t));
    GetIdentifyInfo();
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

int8_t Port::FindCommandSlot(){
	uint32_t slots = (HbaPort->SataActive | HbaPort->CommandIssue);
	for(int8_t i = 0; i < HBA_COMMAND_LIST_MAX_ENTRIES; i++){
		if ((slots & 1) == 0){
			return i;
        }
		slots >>= 1;
	}
	return -1;
}

KResult Port::ReadSectors(uint64_t Sector, uint16_t SectorCount, uintptr_t Buffer){
    atomicAcquire(&Lock, 0);

    // Clear physical buffer
    memset(BufferVirtual, 0x0, SectorCount * 512);

    uint32_t SectorLow = (uint32_t)Sector & 0xFFFFFFFF;
    uint32_t sectorHigh = (uint32_t)(Sector >> 32) & 0xFFFFFFFF;

    HbaPort->InterruptStatus = NULL; // Clear pending interrupt bits

    int8_t slot = FindCommandSlot();
    if(slot == -1){
        atomicUnlock(&Lock, 0);
        return KFAIL;
    }

    CommandHeader->CommandFISLength = sizeof(FisHostToDeviceRegisters_t) / sizeof(uint32_t); // Command FIS size;
    CommandHeader += slot;
    CommandHeader->Atapi = 0;
    CommandHeader->Write = 0; // Read mode
    CommandHeader->PrdtLength = (uint16_t)((SectorCount - 1) >> 4) + 1;

    HBACommandTable_t* CommandTable = CommandAddressTable[0];
    memset(CommandTable, 0, sizeof(HBACommandTable_t) + (CommandHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry_t));

    size64_t Size = SectorCount << 9;
    for(uint16_t i = 0; i < CommandHeader->PrdtLength; i++){
        size64_t SizeInPrdt = Size;
        if(SizeInPrdt > HBA_PRDT_ENTRY_MAX_SIZE){
            SizeInPrdt = HBA_PRDT_ENTRY_MAX_SIZE;
        }
		CommandTable->PrdtEntry[i].DataBaseAddress = (uint64_t)BufferPhysical + i * HBA_PRDT_ENTRY_MAX_SIZE;
        CommandTable->PrdtEntry[i].ByteCount = SizeInPrdt - 1; // 512 bytes per sector
        CommandTable->PrdtEntry[i].InterruptOnCompletion = 1; 
        if(Size == SizeInPrdt){
            break;
        }else{
		    Size -= SizeInPrdt;	
        }
	}

    FisHostToDeviceRegisters_t* CommandFIS = (FisHostToDeviceRegisters_t*)(&CommandTable->CommandFIS);

    CommandFIS->FisType = FISTypeEnum::HostToDevice;
    CommandFIS->CommandControl = 1; // Command
    CommandFIS->Command = ATACommandEnum::ReadDMA; // Read command

    CommandFIS->Lba0 = (uint8_t)SectorLow;
    CommandFIS->Lba1 = (uint8_t)(SectorLow >> 8);
    CommandFIS->Lba2 = (uint8_t)(SectorLow >> 16);
    CommandFIS->Lba3 = (uint8_t)sectorHigh;
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 8);
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 16);

    CommandFIS->DeviceRegister = 1 << 6; // LBA mode

    CommandFIS->CountLow = SectorCount & 0xFF;
    CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

    uint64_t spin = 0;
    while((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        atomicUnlock(&Lock, 0);
        return KFAIL;
    }

    HbaPort->CommandIssue = 1 << slot;

    while (true){
        if((HbaPort->CommandIssue & (1 << slot)) == 0) break;
        if(HbaPort->InterruptStatus & HBA_INTERRUPT_STATU_TFE){
            atomicUnlock(&Lock, 0);
            return KFAIL;
        }
    }

    memcpy(Buffer, BufferVirtual, SectorCount << 9); // Copy data to DMA buffer

    atomicUnlock(&Lock, 0);

    return KSUCCESS;
}

KResult Port::WriteSectors(uint64_t Sector, uint16_t SectorCount, uintptr_t Buffer){
    atomicAcquire(&Lock, 0);

    memcpy(BufferVirtual, Buffer, SectorCount << 9); // Copy data to DMA buffer
    
    uint32_t SectorLow = (uint32_t)Sector & 0xFFFFFFFF;
    uint32_t sectorHigh = (uint32_t)(Sector >> 32) & 0xFFFFFFFF;

    HbaPort->InterruptStatus = NULL; // Clear pending interrupt bits

    int8_t slot = FindCommandSlot();
    if(slot == -1){
        atomicUnlock(&Lock, 0);
        return KFAIL;
    }

    CommandHeader->CommandFISLength = sizeof(FisHostToDeviceRegisters_t) / sizeof(uint32_t); // Command FIS size;
    CommandHeader += slot;
    CommandHeader->Atapi = 0;
    CommandHeader->Write = 1; // Write mode
    CommandHeader->PrdtLength = (uint16_t)((SectorCount - 1) >> 4) + 1;

    HBACommandTable_t* CommandTable = CommandAddressTable[0];
    memset(CommandTable, 0, sizeof(HBACommandTable_t) + (CommandHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry_t));

    size64_t Size = SectorCount << 9;
    for(uint16_t i = 0; i < CommandHeader->PrdtLength; i++){
        size64_t SizeInPrdt = Size;
        if(SizeInPrdt > HBA_PRDT_ENTRY_MAX_SIZE){
            SizeInPrdt = HBA_PRDT_ENTRY_MAX_SIZE;
        }
		CommandTable->PrdtEntry[i].DataBaseAddress = (uint64_t)BufferPhysical + i * HBA_PRDT_ENTRY_MAX_SIZE;
        CommandTable->PrdtEntry[i].ByteCount = SizeInPrdt - 1; // 512 bytes per sector
        CommandTable->PrdtEntry[i].InterruptOnCompletion = 1; 
        if(Size == SizeInPrdt){
            break;
        }else{
		    Size -= SizeInPrdt;	
        }
	}

    FisHostToDeviceRegisters_t* CommandFIS = (FisHostToDeviceRegisters_t*)(&CommandTable->CommandFIS);

    CommandFIS->FisType = FISTypeEnum::HostToDevice;
    CommandFIS->CommandControl = 1; // Command
    CommandFIS->Command = ATACommandEnum::WriteDMA; // Read command

    CommandFIS->Lba0 = (uint8_t)SectorLow;
    CommandFIS->Lba1 = (uint8_t)(SectorLow >> 8);
    CommandFIS->Lba2 = (uint8_t)(SectorLow >> 16);
    CommandFIS->Lba3 = (uint8_t)sectorHigh;
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 8);
    CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 16);

    CommandFIS->DeviceRegister = 1 << 6; // LBA mode

    CommandFIS->CountLow = SectorCount & 0xFF;
    CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

    uint64_t spin = 0;
    while((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
        spin++;
    }
    if(spin == ATA_CMD_TIMEOUT){
        atomicUnlock(&Lock, 0);
        return KFAIL;
    }

    HbaPort->CommandIssue = 1 << slot;

    while (true){
        if((HbaPort->CommandIssue & (1 << slot)) == 0) break;
        if(HbaPort->InterruptStatus & HBA_INTERRUPT_STATU_TFE){
            atomicUnlock(&Lock, 0);
            return KFAIL;
        }
    }

    atomicUnlock(&Lock, 0);
    
    return KSUCCESS;
}

KResult Port::GetIdentifyInfo(){
    atomicAcquire(&Lock, 0);
    // Clear physical buffer
    memset(BufferVirtual, 0x0, sizeof(IdentifyInfo_t));

    HbaPort->InterruptStatus = NULL; // Clear pending interrupt bits

    int8_t slot = FindCommandSlot();
    if(slot == -1){
        return KFAIL;
    }

    CommandHeader->CommandFISLength = sizeof(FisHostToDeviceRegisters_t) / sizeof(uint32_t); // Command FIS size;
    CommandHeader += slot;
    CommandHeader->Atapi = 0;
    CommandHeader->Write = 0; // Read mode
    CommandHeader->PrdtLength = 1;

    HBACommandTable_t* CommandTable = CommandAddressTable[0];
    memset(CommandTable, 0, sizeof(HBACommandTable_t) + (CommandHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry_t));

    CommandTable->PrdtEntry[0].DataBaseAddress = (uint64_t)BufferPhysical;
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
        atomicUnlock(&Lock, 0);
        return KFAIL;
    }

    HbaPort->CommandIssue = 1 << slot;

    while (true){
        if((HbaPort->CommandIssue & (1 << slot)) == 0) break;
        if(HbaPort->InterruptStatus & HBA_INTERRUPT_STATU_TFE){
            atomicUnlock(&Lock, 0);
            return KFAIL;
        }
    }

    memcpy(IdentifyInfo, BufferVirtual, sizeof(IdentifyInfo_t)); // copy data to DMA buffer

    atomicUnlock(&Lock, 0);

    return KSUCCESS;
}

uint64_t Port::GetSize(){
    return (IdentifyInfo->TotalNumberUserAddressableLBASectorsAvailable << 9);
}

uint16_t* Port::GetModelNumber(){
    return IdentifyInfo->DriveModelNumber;
}

uint16_t* Port::GetSerialNumber(){
    return IdentifyInfo->SerialNumber;
}

KResult Port::Read(uint64_t Start, size64_t Size, uintptr_t Buffer){
    // alignement 
    uint64_t StaterByteIteration = Start;
    uint64_t SizeByte = Size;
    uint64_t EndByte = StaterByteIteration + SizeByte;
    uint64_t BufferInteration = (uint64_t)Buffer;
    uint16_t Alignement = Start & 0x1FF;
    uint16_t AlignementFill = 0x200 - Alignement;

    uintptr_t SectorBuffer = malloc(BufferSize);
    if(Alignement){
        ReadSectors(Alignement, 1, SectorBuffer);
        memcpy((uintptr_t)BufferInteration, (uintptr_t)((uint64_t)SectorBuffer + Alignement), AlignementFill);
        BufferInteration += AlignementFill;
        StaterByteIteration += AlignementFill;
        SizeByte -= AlignementFill;
    }
    for(size64_t i = StaterByteIteration; StaterByteIteration < EndByte; i += BufferSize){
        SizeToCopy = SizeByte;
        if(SizeToCopy > BufferSize){
            SizeToCopy = BufferSize;
        }
        ReadSectors(Alignement, 1, SectorBuffer);
        memcpy((uintptr_t)BufferInteration, (uintptr_t)((uint64_t)SectorBuffer, SizeToCopy);
        BufferInteration += SizeToCopy;
        StaterByteIteration += SizeToCopy;
        SizeByte -= SizeToCopy;
    }

}

KResult Port::Write(uint64_t Start, size64_t Size, uintptr_t Buffer){

}