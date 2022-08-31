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
    BufferSize = 0x1000 * 0x100; // 1 mo
    BufferVirtual = getFreeAlignedSpace(BufferSize);

    // Be sur to unlock the locker
    atomicUnlock(&Lock, 0);

    // Identify disk
    IdentifyInfo = (IdentifyInfo_t*)calloc(sizeof(IdentifyInfo_t));
    GetIdentifyInfo();

    std::printf("%x", GetSize());

    uint8_t* buffer = (uint8_t*)malloc(0x200);
    Read(0x0, 0x10, buffer);

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

KResult Port::Read(uint64_t Start, size64_t Size, uintptr_t Buffer){
    uint64_t StaterByteIteration = Start;
    uint64_t SizeByte = Size;
    uint64_t EndByte = StaterByteIteration + SizeByte;
    uint64_t BufferInteration = (uint64_t)Buffer;
    uint16_t AlignementStart = Start & 0x1FF;
    uint16_t AlignementStartFill = 0x200 - AlignementStart;
    uint16_t AlignementEnd = EndByte & 0x1FF;
    uint16_t AlignementEndFill = 0x200 - AlignementEnd;

    uint64_t SectorIteration = Start & ~((uint64_t)0x1FF);

    uint64_t BlockCount = divideRoundUp(Size + AlignementStart, BufferSize);

    uint64_t SectorToRead = divideRoundUp(BlockCount, ATA_SECTOR_SIZE);

    for(size64_t i = 0; i < BlockCount; i++){
        uint64_t SectorCount = SectorToRead;
        if((SectorCount << 9) > BufferSize){
            SectorCount = BufferSize >> 9;
        }

        atomicAcquire(&Lock, 0);

        uint32_t SectorLow = (uint32_t)SectorIteration & 0xFFFFFFFF;
        uint32_t sectorHigh = (uint32_t)(SectorIteration >> 32) & 0xFFFFFFFF;

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
        // Give buffer
        size64_t Size = SectorCount << 9;
        for(uint16_t y = 0; y < CommandHeader->PrdtLength; y++){
            size64_t SizeToLoadInPrdt = Size;
            if(SizeToLoadInPrdt > HBA_PRDT_ENTRY_MAX_SIZE){
                SizeToLoadInPrdt = HBA_PRDT_ENTRY_MAX_SIZE;
            }
            uintptr_t BufferDst = (uintptr_t)((uint64_t)BufferVirtual + y * HBA_PRDT_ENTRY_MAX_SIZE);
            MapPhysicalToVirtual(BufferDst, (uintptr_t*)&CommandTable->PrdtEntry[i].DataBaseAddress, HBA_PRDT_ENTRY_MAX_SIZE);
            CommandTable->PrdtEntry[i].ByteCount = SizeToLoadInPrdt - 1; // 512 bytes per sector
            CommandTable->PrdtEntry[i].InterruptOnCompletion = 1; 
            if(Size == SizeToLoadInPrdt){
                break;
            }else{
                Size -= SizeToLoadInPrdt;	
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
                // Free buffer
                size64_t SizeToFree = SectorCount << 9;
                for(uint16_t y = 0; y < CommandHeader->PrdtLength; y++){
                    size64_t SizeInPrdt = SizeToFree;
                    SizeInPrdt -= AlignementStart;
                    if(SizeInPrdt > HBA_PRDT_ENTRY_MAX_SIZE){
                        SizeInPrdt = HBA_PRDT_ENTRY_MAX_SIZE;
                    }
                    uintptr_t BufferSrc = (uintptr_t)((uint64_t)BufferVirtual + y * HBA_PRDT_ENTRY_MAX_SIZE);
                    FreeAddress(BufferSrc, SizeInPrdt);
                    if(SizeToFree == SizeInPrdt){
                        break;
                    }else{
                        SizeToFree -= SizeInPrdt;	
                    }
                }
                atomicUnlock(&Lock, 0);
                return KFAIL;
            }
        }

        // Get buffer and free
        size64_t SizeToCopy = SectorCount << 9;
        for(uint16_t y = 0; y < CommandHeader->PrdtLength; y++){
            size64_t SizeInPrdt = SizeToCopy;
            SizeInPrdt -= AlignementStart;
            if(SizeInPrdt > HBA_PRDT_ENTRY_MAX_SIZE){
                SizeInPrdt = HBA_PRDT_ENTRY_MAX_SIZE;
            }
            uintptr_t BufferSrc = (uintptr_t)((uint64_t)BufferVirtual + y * HBA_PRDT_ENTRY_MAX_SIZE);
            uintptr_t BufferDst = (uintptr_t)((uint64_t)BufferInteration + y * HBA_PRDT_ENTRY_MAX_SIZE);
            memcpy(BufferDst, (uintptr_t)((uint64_t)BufferSrc + (uint64_t)AlignementStart), SizeInPrdt);
            FreeAddress(BufferSrc, SizeInPrdt);
            if(SizeToCopy == SizeInPrdt){
                break;
            }else{
                SizeToCopy -= SizeInPrdt;	
            }
        }

        atomicUnlock(&Lock, 0);

        SectorToRead -= SectorCount;
        SectorIteration += SectorCount;
        BufferInteration += (SectorCount << 9);
    }


    return KSUCCESS;
}

KResult Port::Write(uint64_t Start, size64_t Size, uintptr_t Buffer){
    // atomicAcquire(&Lock, 0);

    // memcpy(BufferVirtual, Buffer, SectorCount << 9); // Copy data to DMA buffer
    
    // uint32_t SectorLow = (uint32_t)Sector & 0xFFFFFFFF;
    // uint32_t sectorHigh = (uint32_t)(Sector >> 32) & 0xFFFFFFFF;

    // HbaPort->InterruptStatus = NULL; // Clear pending interrupt bits

    // int8_t slot = FindCommandSlot();
    // if(slot == -1){
    //     atomicUnlock(&Lock, 0);
    //     return KFAIL;
    // }

    // CommandHeader->CommandFISLength = sizeof(FisHostToDeviceRegisters_t) / sizeof(uint32_t); // Command FIS size;
    // CommandHeader += slot;
    // CommandHeader->Atapi = 0;
    // CommandHeader->Write = 1; // Write mode
    // CommandHeader->PrdtLength = (uint16_t)((SectorCount - 1) >> 4) + 1;

    // HBACommandTable_t* CommandTable = CommandAddressTable[0];
    // memset(CommandTable, 0, sizeof(HBACommandTable_t) + (CommandHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry_t));

    // size64_t Size = SectorCount << 9;
    // for(uint16_t i = 0; i < CommandHeader->PrdtLength; i++){
    //     size64_t SizeInPrdt = Size;
    //     if(SizeInPrdt > HBA_PRDT_ENTRY_MAX_SIZE){
    //         SizeInPrdt = HBA_PRDT_ENTRY_MAX_SIZE;
    //     }
	// 	CommandTable->PrdtEntry[i].DataBaseAddress = (uint64_t)BufferPhysical + i * HBA_PRDT_ENTRY_MAX_SIZE;
    //     CommandTable->PrdtEntry[i].ByteCount = SizeInPrdt - 1; // 512 bytes per sector
    //     CommandTable->PrdtEntry[i].InterruptOnCompletion = 1; 
    //     if(Size == SizeInPrdt){
    //         break;
    //     }else{
	// 	    Size -= SizeInPrdt;	
    //     }
	// }

    // FisHostToDeviceRegisters_t* CommandFIS = (FisHostToDeviceRegisters_t*)(&CommandTable->CommandFIS);

    // CommandFIS->FisType = FISTypeEnum::HostToDevice;
    // CommandFIS->CommandControl = 1; // Command
    // CommandFIS->Command = ATACommandEnum::WriteDMA; // Read command

    // CommandFIS->Lba0 = (uint8_t)SectorLow;
    // CommandFIS->Lba1 = (uint8_t)(SectorLow >> 8);
    // CommandFIS->Lba2 = (uint8_t)(SectorLow >> 16);
    // CommandFIS->Lba3 = (uint8_t)sectorHigh;
    // CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 8);
    // CommandFIS->Lba4 = (uint8_t)(sectorHigh >> 16);

    // CommandFIS->DeviceRegister = 1 << 6; // LBA mode

    // CommandFIS->CountLow = SectorCount & 0xFF;
    // CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

    // uint64_t spin = 0;
    // while((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_FIS_DRQ)) && spin < ATA_CMD_TIMEOUT){
    //     spin++;
    // }
    // if(spin == ATA_CMD_TIMEOUT){
    //     atomicUnlock(&Lock, 0);
    //     return KFAIL;
    // }

    // HbaPort->CommandIssue = 1 << slot;

    // while (true){
    //     if((HbaPort->CommandIssue & (1 << slot)) == 0) break;
    //     if(HbaPort->InterruptStatus & HBA_INTERRUPT_STATU_TFE){
    //         atomicUnlock(&Lock, 0);
    //         return KFAIL;
    //     }
    // }

    // atomicUnlock(&Lock, 0);
    
    // return KSUCCESS;
}

KResult Port::GetIdentifyInfo(){
    atomicAcquire(&Lock, 0);

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

    MapPhysicalToVirtual(BufferVirtual, (uintptr_t*)&CommandTable->PrdtEntry[0].DataBaseAddress, sizeof(IdentifyInfo_t));
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
    FreeAddress(BufferVirtual, sizeof(IdentifyInfo_t));

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