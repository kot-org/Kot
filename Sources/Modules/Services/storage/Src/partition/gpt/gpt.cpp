#include <partition/gpt/gpt.h>

KResult CheckGPTHeader(GPTHeader_t* GPTHeader){
    if(GPTHeader->Signature == GPT_SIGNATURE){
        uint32_t crc32HeaderTMP = GPTHeader->HeaderCRC32;
        GPTHeader->HeaderCRC32 = 0;
        uint32_t crc32HeaderCompute = crc32(NULL, (char*)GPTHeader, sizeof(GPTHeader_t));
        GPTHeader->HeaderCRC32 = crc32HeaderTMP;

        if(GPTHeader->HeaderCRC32 == crc32HeaderCompute){
            return KSUCCESS;
        }else{
            return KMEMORYVIOLATION;
        }
    }

    return KFAIL;
}

KResult device_partitions_t::LoadGPTHeader(){
    if(IsGPTDisk()){
        if(!GPTHeader){
            GPTHeader = (GPTHeader_t*)malloc(sizeof(GPTHeader_t));
        }

        uint64_t GPTHeaderLBAStart = MBRHeader->PartitionRecord[0].StartingLBA;
        Device->ReadDevice(GPTHeader, ConvertLBAToBytes(GPTHeaderLBAStart), sizeof(GPTHeader_t));
        if(GPTHeader->MyLBA != GPTHeaderLBAStart){
            return KNOTALLOW;
        }
        KResult GPTHeaderStatus = CheckGPTHeader(GPTHeader);
        // Load recovery header
        GPTHeader_t* RecoveryGPTHeader = (GPTHeader_t*)malloc(sizeof(GPTHeader_t));
        Device->ReadDevice(RecoveryGPTHeader, ConvertLBAToBytes(GPTHeader->AlternateLBA), sizeof(GPTHeader_t));
        KResult GPTRecoveryHeaderStatus = CheckGPTHeader(RecoveryGPTHeader);

        if(GPTHeaderStatus != KSUCCESS){
            if(GPTRecoveryHeaderStatus == KSUCCESS){
                // Update my lba
                RecoveryGPTHeader->MyLBA = GPTHeaderLBAStart; 

                // Update checksum
                RecoveryGPTHeader->HeaderCRC32 = 0;
                RecoveryGPTHeader->HeaderCRC32 = crc32(NULL, (char*)RecoveryGPTHeader, sizeof(GPTHeader_t));

                // Update GPT Header into the disk
                Device->WriteDevice(RecoveryGPTHeader, ConvertLBAToBytes(GPTHeaderLBAStart), sizeof(GPTHeader_t));

                // Update GPT Header
                Device->ReadDevice(GPTHeader, ConvertLBAToBytes(GPTHeaderLBAStart), sizeof(GPTHeader_t));
            }else{
                free(RecoveryGPTHeader);
                return KMEMORYVIOLATION;
            }
        }else if(GPTRecoveryHeaderStatus != KSUCCESS){
            // Update my lba
            RecoveryGPTHeader->MyLBA = GPTHeader->AlternateLBA; 

            // Update checksum
            RecoveryGPTHeader->HeaderCRC32 = 0;
            RecoveryGPTHeader->HeaderCRC32 = crc32(NULL, (char*)RecoveryGPTHeader, sizeof(GPTHeader_t));

            // Update GPT Recovery Header into the disk
            Device->WriteDevice(RecoveryGPTHeader, ConvertLBAToBytes(GPTHeader->AlternateLBA), sizeof(GPTHeader_t));
        }

        if(GPTHeaderStatus == KSUCCESS){
            IsGPTHeaderLoaded = true;
            free(RecoveryGPTHeader);
            return KSUCCESS;
        }
        free(RecoveryGPTHeader);      
    }
    return KFAIL;
}

uint64_t device_partitions_t::CheckPartitions(){
    size64_t SizeOfPartitionList = sizeof(GPTPartitionEntry_t) * GPTHeader->NumberOfPartitionEntries;

    uint32_t crc32HeaderCompute = crc32(NULL, (char*)GPTPartitionEntries, SizeOfPartitionList);

    // Check recovery 
    GPTPartitionEntry_t* GPTPartitionEntriesRecovery = (GPTPartitionEntry_t*)malloc(SizeOfPartitionList);
    uint64_t PGTPartitionEntriesRevoryLocation = ConvertLBAToBytes(GPTHeader->AlternateLBA) - SizeOfPartitionList;
    Device->ReadDevice(GPTPartitionEntriesRecovery, PGTPartitionEntriesRevoryLocation, SizeOfPartitionList);
    uint32_t crc32RecoveryHeaderCompute = crc32(NULL, (char*)GPTPartitionEntriesRecovery, SizeOfPartitionList);

    if(GPTHeader->PartitionEntryArrayCRC32 == crc32HeaderCompute){
        if(GPTHeader->PartitionEntryArrayCRC32 != crc32RecoveryHeaderCompute){
            // Update GPT Recovery partition entries into the disk
            Device->WriteDevice(GPTPartitionEntries, PGTPartitionEntriesRevoryLocation, SizeOfPartitionList);
        }
        free(GPTPartitionEntriesRecovery);
        return KSUCCESS;
    }else{
        if(GPTHeader->PartitionEntryArrayCRC32 == crc32RecoveryHeaderCompute){
            // Update GPT partition entries into the disk
            Device->WriteDevice(GPTPartitionEntriesRecovery, ConvertLBAToBytes(GPTHeader->PartitionEntryLBA), SizeOfPartitionList);

            // Update entries
            Device->ReadDevice(GPTPartitionEntries, ConvertLBAToBytes(GPTHeader->PartitionEntryLBA), SizeOfPartitionList);

            free(GPTPartitionEntriesRecovery);
            return KSUCCESS;
        }
    }

    return KFAIL;
}

KResult device_partitions_t::LoadPartitions(){
    if(!IsGPTHeaderLoaded){
        if(LoadGPTHeader() != KSUCCESS){
            return KFAIL;
        }
    }
    
    // Load partitions
    size64_t SizeOfPartitionList = sizeof(GPTPartitionEntry_t) * GPTHeader->NumberOfPartitionEntries;
    if(!GPTPartitionEntries){
        GPTPartitionEntries = (GPTPartitionEntry_t*)malloc(SizeOfPartitionList);
    }
    
    Device->ReadDevice(GPTPartitionEntries, ConvertLBAToBytes(GPTHeader->PartitionEntryLBA), SizeOfPartitionList);
    if(CheckPartitions() == KSUCCESS){
        IsGPTPartitionsLoaded = true;
        for(uint64_t i = 0; i < GPT_MAX_PARTITIONS; i++){
            uint64_t Start = ConvertLBAToBytes(GPTPartitionEntries[i].StartingLBA);
            uint64_t Size = ConvertLBAToBytes(GPTPartitionEntries[i].EndingLBA - GPTPartitionEntries[i].StartingLBA);
            if(Size != NULL){
                NewPartition(Device, Start, Size, &GPTPartitionEntries[i].PartitionTypeGUID);
            }
        }
        return KSUCCESS;
    }
    
    return KFAIL;
}