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

        KResult GPTHeaderStatu = CheckGPTHeader(GPTHeader);
        if(GPTHeaderStatu != KSUCCESS){
            GPTHeader_t* RecoveryGPTHeader = (GPTHeader_t*)malloc(sizeof(GPTHeader_t));
            Device->ReadDevice(RecoveryGPTHeader, ConvertLBAToBytes(GPTHeader->AlternateLBA), sizeof(GPTHeader_t));
            GPTHeaderStatu = CheckGPTHeader(RecoveryGPTHeader);
            if(GPTHeaderStatu == KSUCCESS){
                // Update my lba
                RecoveryGPTHeader->MyLBA = GPTHeaderLBAStart; 

                // Update checksum
                RecoveryGPTHeader->HeaderCRC32 = 0;
                RecoveryGPTHeader->HeaderCRC32 = crc32(NULL, (char*)RecoveryGPTHeader, sizeof(GPTHeader_t));


                Device->WriteDevice(RecoveryGPTHeader, ConvertLBAToBytes(GPTHeaderLBAStart), sizeof(GPTHeader_t));
                Device->ReadDevice(GPTHeader, ConvertLBAToBytes(GPTHeaderLBAStart), sizeof(GPTHeader_t));
                free(RecoveryGPTHeader);
            }else{
                free(RecoveryGPTHeader);
                return KMEMORYVIOLATION;
            }
        }

        if(GPTHeaderStatu == KSUCCESS){
            IsGPTHeaderLoaded = true;
            return KSUCCESS;
        }        
    }
    return KFAIL;
}

uint64_t device_partitions_t::CheckPartitions(){
    size64_t SizeOfPartitionList = sizeof(GPTPartitionEntry_t) * GPTHeader->NumberOfPartitionEntries;

    uint32_t crc32HeaderCompute = crc32(NULL, (char*)GPTPartitionEntries, SizeOfPartitionList);

    if(GPTHeader->PartitionEntryArrayCRC32 == crc32HeaderCompute){
        return KSUCCESS;
    }else{
        GPTPartitionEntry_t* GPTPartitionEntriesRecovery = (GPTPartitionEntry_t*)malloc(SizeOfPartitionList);
        uint64_t PGTPartitionEntriesRevoryLocation = ConvertLBAToBytes(GPTHeader->AlternateLBA) - SizeOfPartitionList;
        Device->ReadDevice(GPTPartitionEntriesRecovery, PGTPartitionEntriesRevoryLocation, SizeOfPartitionList);
        crc32HeaderCompute = crc32(NULL, (char*)GPTPartitionEntries, SizeOfPartitionList);
        if(GPTHeader->PartitionEntryArrayCRC32 == crc32HeaderCompute){
            Device->WriteDevice(GPTPartitionEntriesRecovery, ConvertLBAToBytes(GPTHeader->PartitionEntryLBA), SizeOfPartitionList);
            Device->ReadDevice(GPTPartitionEntries, ConvertLBAToBytes(GPTHeader->PartitionEntryLBA), SizeOfPartitionList);
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
        return KSUCCESS;
    }
    
    return KFAIL;
}