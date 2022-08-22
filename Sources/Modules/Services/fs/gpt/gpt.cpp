#include "gpt.h"
#include "../kfs/kfs.h"

#define SectorSizeLBA 512

namespace GPT{

    PartitionsInfo** AllPartitionsInfo;
    uint64_t AllPartitionsInfoNumber;

    GPTHeader* GetGPTHeader(AHCI::Port* port){
        memset(globalPageTableManager[0].GetVirtualAddress(port->Buffer), 0, sizeof(GPTHeader));
        if(port->Read(1, 1, port->Buffer)){            
            GPTHeader* ReturnValue = (GPTHeader*)malloc(sizeof(GPTHeader));
            memcpy(ReturnValue, globalPageTableManager[0].GetVirtualAddress(port->Buffer), sizeof(GPTHeader));
            return ReturnValue;           
        }else{
            return NULL;
        }        
    }

    bool SetGPTHeader(AHCI::Port* port, GPTHeader* newGPTHeader){
        memcpy(globalPageTableManager[0].GetVirtualAddress(port->Buffer), newGPTHeader, sizeof(GPTHeader));
        port->Write(1, 1, port->Buffer);     
        port->Write(newGPTHeader->BackupLBA, 1, port->Buffer);
    }

    void InitGPTHeader(AHCI::Port* port){
        /* https://fr.wikipedia.org/wiki/GUID_Partition_Table */
        
        GPTHeader* gptheader = (GPTHeader*)malloc(sizeof(GPTHeader));
        
        //efi part signature
        gptheader->Signature[0] = 'E'; 
        gptheader->Signature[1] = 'F'; 
        gptheader->Signature[2] = 'I'; 
        gptheader->Signature[3] = ' '; 
        gptheader->Signature[4] = 'P'; 
        gptheader->Signature[5] = 'A'; 
        gptheader->Signature[6] = 'R'; 
        gptheader->Signature[7] = 'T'; 
        
        //version
        gptheader->Revision[0] = 0;
        gptheader->Revision[1] = 0;
        gptheader->Revision[2] = 1;
        gptheader->Revision[3] = 0;

        //HeaderSize
        gptheader->HeaderSize = sizeof(GPTHeader);

        //CRC32
        gptheader->CRC32 = 0;

        //Reserved
        gptheader->Reserved = 0;

        //CurrentLBA
        gptheader->CurrentLBA = 1; //first sector

        //BackupLBA
        gptheader->BackupLBA = port->GetNumberSectorsLBA() - 2;

        //FirstUsableLBAPartitions
        gptheader->FirstUsableLBAPartitions = (((sizeof(GUIDPartitionEntryFormat) * MaxParitionsNumber) / SectorSizeLBA) + (gptheader->CurrentLBA + 1)) ; //we add 1 to current lba because we can't write on them

        //LastUsableLBAPartitions
        gptheader->LastUsableLBAPartitions = port->GetNumberSectorsLBA() - gptheader->FirstUsableLBAPartitions;

        //DiskGUID
        GUID* DiskGUID = (GUID*)malloc(sizeof(GUID));
        DiskGUID->Data1 = (uint32_t)port->DiskInfo->SerialNumber[0] & port->DiskInfo->SerialNumber[9];
        DiskGUID->Data2 = (uint16_t)port->DiskInfo->DriveModelNumber[0] & port->DiskInfo->DriveModelNumber[19];
        DiskGUID->Data3 = (uint16_t)port->DiskInfo->DriveModelNumber[9];
        DiskGUID->Data4 = (uint64_t)port->DiskInfo->SerialNumber[0] & port->DiskInfo->SerialNumber[3] & port->DiskInfo->SerialNumber[6] & port->DiskInfo->SerialNumber[9];
        gptheader->DiskGUID = *DiskGUID;

        //PartitionEntriesStartingLBA
        gptheader->PartitionEntriesStartingLBA = gptheader->CurrentLBA + 1;


        //NumberPartitionEntries
        gptheader->NumberPartitionEntries = MaxParitionsNumber;

        //SizePartitionEntry
        gptheader->SizePartitionEntry = sizeof(GUIDPartitionEntryFormat);

        gptheader->CRC32PartitionArray = 0;

        SetGPTHeader(port, gptheader);
        free(DiskGUID);
        free(gptheader);
    }

    GUIDPartitionEntryFormat* GetGUIDPartitionEntryFormat(AHCI::Port* port, uint64_t LBAAddress, uint8_t which){
        uint8_t MaxGUIDPartitionEntryFormatPerSectors = SectorSizeLBA / sizeof(GUIDPartitionEntryFormat);
        memset(globalPageTableManager[0].GetVirtualAddress(port->Buffer), 0, SectorSizeLBA);

        uintptr_t buffer = malloc(sizeof(GUIDPartitionEntryFormat));
        memset(buffer, 0, sizeof(GUIDPartitionEntryFormat));

        if(port->Read(LBAAddress, 1, port->Buffer)){
            memcpy(buffer, globalPageTableManager[0].GetVirtualAddress(port->Buffer + (which * sizeof(GUIDPartitionEntryFormat))), sizeof(GUIDPartitionEntryFormat));
            GUIDPartitionEntryFormat* ReturnValue = (GUIDPartitionEntryFormat*)buffer;
            return ReturnValue;
        }else{
            free(buffer);
            return NULL;
        }        
    }

    bool SetGUIDPartitionEntryFormat(AHCI::Port* port, uint64_t LBAAddress, uint8_t which, GUIDPartitionEntryFormat* newGuidPartitionEntryFormat, GPTHeader* GptHeader){
        uint8_t MaxGUIDPartitionEntryFormatPerSectors = SectorSizeLBA / sizeof(GUIDPartitionEntryFormat);
        memcpy(globalPageTableManager[0].GetVirtualAddress(port->Buffer), newGuidPartitionEntryFormat, sizeof(GUIDPartitionEntryFormat));

        if(port->Read(LBAAddress, 1, port->Buffer)){
            memcpy(globalPageTableManager[0].GetVirtualAddress(port->Buffer + (which * sizeof(GUIDPartitionEntryFormat))), newGuidPartitionEntryFormat, sizeof(GUIDPartitionEntryFormat));

            port->Write(LBAAddress, 1, port->Buffer);  
        
            memcpy(globalPageTableManager[0].GetVirtualAddress(port->Buffer + (which * sizeof(GUIDPartitionEntryFormat))), newGuidPartitionEntryFormat, sizeof(GUIDPartitionEntryFormat));
            return port->Write(GptHeader->LastUsableLBAPartitions + (LBAAddress - (GptHeader->CurrentLBA + 1)), 1, port->Buffer);
        }else{
            return false;
        }  
    } 
    
    uint64_t GetFirstFreeLBA(AHCI::Port* port){
        GPTHeader* gptHeader = GetGPTHeader(port);
        Partitions* partitions = GetAllPartitions(port);
        uint64_t MaxLastUsedLBA = 0;
        if(partitions->NumberPartitionsCreated == 0){
            
            return gptHeader->FirstUsableLBAPartitions;
        }
        for(int i = 0; i < partitions->NumberPartitionsCreated; i++){
            if(partitions->AllParitions[i]->LastLBA > MaxLastUsedLBA){
                MaxLastUsedLBA = partitions->AllParitions[i]->LastLBA;
            }
        }
        return MaxLastUsedLBA + 1; //because the last is used so we need to return the free
    }

    GUIDPartitionEntryFormat* GetPartitionByGUID(AHCI::Port* port, GUID* guid){
        Partitions* AllPartitions = GetAllPartitions(port);

        for(int i = 0; i < AllPartitions->NumberPartitionsCreated; i++){
            if(AllPartitions->AllParitions[i]->PartitionTypeGUID.Data1 == guid->Data1 &&
                AllPartitions->AllParitions[i]->PartitionTypeGUID.Data2 == guid->Data2 &&
                AllPartitions->AllParitions[i]->PartitionTypeGUID.Data3 == guid->Data3 &&
                AllPartitions->AllParitions[i]->PartitionTypeGUID.Data4 == guid->Data4){
                return AllPartitions->AllParitions[i];
            }
        }
        return NULL;
    }

    Partitions* GetAllPartitions(AHCI::Port* port){  
        GPTHeader* gptHeader = GetGPTHeader(port);
        Partitions* ReturnValue = (Partitions*)malloc(sizeof(Partitions));

        memset(ReturnValue, 0, sizeof(Partitions));
        ReturnValue->IsPartitionsEntryBitmapFree = BitmapHeap(gptHeader->NumberPartitionEntries);

        uint8_t MaxGUIDPartitionEntryFormatPerSectors = SectorSizeLBA / sizeof(GUIDPartitionEntryFormat);
        uint64_t PartitionEntriesStartingLBA = gptHeader->PartitionEntriesStartingLBA;
        GUIDPartitionEntryFormat* CheckEntry = (GUIDPartitionEntryFormat*)malloc(sizeof(GUIDPartitionEntryFormat));;
        for(int i = 0; i < gptHeader->NumberPartitionEntries; i++){
            CheckEntry = (GUIDPartitionEntryFormat*)malloc(sizeof(GUIDPartitionEntryFormat));
            CheckEntry = GetGUIDPartitionEntryFormat(port, PartitionEntriesStartingLBA + (i / MaxGUIDPartitionEntryFormatPerSectors), i % MaxGUIDPartitionEntryFormatPerSectors);
            if(CheckEntry->FirstLBA != 0){
                ReturnValue->AllParitions[ReturnValue->NumberPartitionsCreated] = CheckEntry;
                ReturnValue->NumberPartitionsCreated++;
                ReturnValue->IsPartitionsEntryBitmapFree.Set(i, false);
            }else{
                ReturnValue->IsPartitionsEntryBitmapFree.Set(i, true);
                free(CheckEntry);
            }
        }

        return ReturnValue;
    }

    bool CreatePartition(AHCI::Port* port, size64_t size, char* PartitionName, GUID* PartitionTypeGUID, uint64_t flags){
        GPTHeader* gptHeader = GetGPTHeader(port);
        Partitions* partitions = GetAllPartitions(port);


        if(partitions->NumberPartitionsCreated >= gptHeader->NumberPartitionEntries){
            return false;
        }        
        

        uint64_t sizeLBA = size / SectorSizeLBA;
        uint8_t MaxGUIDPartitionEntryFormatPerSectors = SectorSizeLBA / sizeof(GUIDPartitionEntryFormat);
        uint64_t PartitionEntriesStartingLBA = gptHeader->PartitionEntriesStartingLBA;

        uint64_t UsedLBASectors = 0;
        uint64_t TotalUsableLBASectors = gptHeader->LastUsableLBAPartitions - gptHeader->FirstUsableLBAPartitions;

        for(int i = 0; i < partitions->NumberPartitionsCreated; i++){
            UsedLBASectors += partitions->AllParitions[i]->LastLBA - partitions->AllParitions[i]->FirstLBA;
        }   

        //check size
        if(UsedLBASectors + sizeLBA < TotalUsableLBASectors){
            
            for(int i = 0; i < gptHeader->NumberPartitionEntries; i++){                
                if(partitions->IsPartitionsEntryBitmapFree.Get(i) == true){
                    GUIDPartitionEntryFormat* newGuidPartitionEntryFormat = (GUIDPartitionEntryFormat*)malloc(sizeof(GUIDPartitionEntryFormat));


                    uint64_t partitionFirstLBA;
                    
                    partitionFirstLBA = GetFirstFreeLBA(port);
                    
                    newGuidPartitionEntryFormat->FirstLBA = partitionFirstLBA;
                    newGuidPartitionEntryFormat->LastLBA = newGuidPartitionEntryFormat->FirstLBA + sizeLBA;
                    newGuidPartitionEntryFormat->Flags = flags;

                    int count = 0;

                    while(*PartitionName != '\0'){
                        newGuidPartitionEntryFormat->PartitionName[count] = *PartitionName;
                        PartitionName++;
                        count++;      
                        if(count >= 36) break;                    
                    }
                    for(int i = count; i < 36; i++){
                        newGuidPartitionEntryFormat->PartitionName[i] = '\0';
                    }

                    newGuidPartitionEntryFormat->PartitionTypeGUID = *PartitionTypeGUID;

                    GUID* UniquePartitionGUID = (GUID*)malloc(sizeof(GUID));

                    UniquePartitionGUID->Data1 = (uint32_t)port->DiskInfo->SerialNumber[0] & port->DiskInfo->SerialNumber[9];
                    UniquePartitionGUID->Data2 = (uint16_t)port->DiskInfo->SerialNumber[5];
                    UniquePartitionGUID->Data3 = (uint16_t)port->DiskInfo->SerialNumber[7];
                    UniquePartitionGUID->Data4 = partitionFirstLBA;

                    newGuidPartitionEntryFormat->UniquePartitionGUID = *UniquePartitionGUID;
                    
                    SetGUIDPartitionEntryFormat(port, PartitionEntriesStartingLBA + (i / MaxGUIDPartitionEntryFormatPerSectors), i % MaxGUIDPartitionEntryFormatPerSectors, newGuidPartitionEntryFormat, gptHeader);
                    
                    free(newGuidPartitionEntryFormat);
                    return true;                   
                }
            }
        }else{
            return false;
        }

        return false;
    }

    uint64_t GetFreeSizePatition(AHCI::Port* port){
        GPTHeader* gptHeader = GetGPTHeader(port);
        Partitions* partitions = GetAllPartitions(port);

        uint64_t UsedLBASectors = 0;
        uint64_t TotalUsableLBASectors = gptHeader->LastUsableLBAPartitions - gptHeader->FirstUsableLBAPartitions;

        for(int i = 0; i < partitions->NumberPartitionsCreated; i++){
            //the problem come from here
            UsedLBASectors += partitions->AllParitions[i]->LastLBA - partitions->AllParitions[i]->FirstLBA;
        }
        uint64_t freeSizeSectors = TotalUsableLBASectors - UsedLBASectors;   
        uint64_t freeSize = (freeSizeSectors * SectorSizeLBA) - 1;
        return freeSize;
    }


    GUID* GetReservedGUIDPartitionType(){
        GUID* KotReservedGUID = (GUID*)malloc(sizeof(GUID));
        KotReservedGUID->Data1 = 0x47A1ACC0;
        KotReservedGUID->Data2 = 0x3B40;
        KotReservedGUID->Data3 = 0x2A53;
        KotReservedGUID->Data4 = 0xF38D3D321F6D;
        return KotReservedGUID;
    }

    GUID* GetDataGUIDPartitionType(){
        GUID* KotReservedGUID = (GUID*)malloc(sizeof(GUID));
        KotReservedGUID->Data1 = 0x64617461;
        KotReservedGUID->Data2 = 0x3B40;
        KotReservedGUID->Data3 = 0x2A53;
        KotReservedGUID->Data4 = 0xF38D3D321F6D;
        return KotReservedGUID;
    }

    GUID* GetSystemGUIDPartitionType(){
        GUID* KotReservedGUID = (GUID*)malloc(sizeof(GUID));
        KotReservedGUID->Data1 = 0xC12A7328;
        KotReservedGUID->Data2 = 0xF81F;
        KotReservedGUID->Data3 = 0x11D2;
        KotReservedGUID->Data4 = 0x00A0C93EC93B;
        return KotReservedGUID;
    }

    void AssignNamePartitionsGUID(){
        bool systemPartInit = false;
        uint64_t PatitionNumber = 0;
        for(int i = 0; i < AllPartitionsInfoNumber; i++){
            if(AllPartitionsInfo[i]->Partition->Flags == 7){ //flags disk system
                AllPartitionsInfo[i]->PatitionNumber = 0;
                systemPartInit = true;
            }else if(AllPartitionsInfo[i]->Partition->Flags == 5){ //flags to mount the disk
                PatitionNumber++;
                AllPartitionsInfo[i]->PatitionNumber = PatitionNumber;                
            }
        }
    }

    /* Partition class */
    
    Partition::Partition(AHCI::Port* port, GUIDPartitionEntryFormat* partition){
        this->partition = partition;
        this->port = port;
    }

    bool Partition::Read(uint64_t firstByte, size64_t size, uintptr_t buffer){
        uint64_t LBAFirstSector = this->partition->FirstLBA + (firstByte / SectorSizeLBA);        
        bool Check;

        memset(buffer, 0, size);
        uint64_t sizeRead = 0;
        uint64_t sizeToRead = 0;
        uint64_t sectorsToRead = 0;
        uint64_t sectorsRead = 0;
        
        for(int i = 0; i < DivideRoundUp(size, port->BufferSize); i++){            
            sizeToRead = size - sizeRead;
            if(sizeToRead > port->BufferSize){
                sizeToRead = port->BufferSize;
            }
            sectorsToRead = DivideRoundUp(sizeToRead, SectorSizeLBA);

            Check = port->Read(LBAFirstSector + sectorsRead, sectorsToRead, port->Buffer);
            if(sizeRead != 0){
                memcpy((uintptr_t)((uint64_t)buffer + sizeRead), globalPageTableManager[0].GetVirtualAddress(port->Buffer), sizeToRead);
            }else{
                memcpy(buffer, globalPageTableManager[0].GetVirtualAddress((uintptr_t)((uint64_t)port->Buffer + firstByte % SectorSizeLBA)), sizeToRead); //Get the correct first byte
            }
            sizeRead += sizeToRead;
            sectorsRead += sectorsToRead;
        }
        
        return Check;
    }

    bool Partition::Write(uint64_t firstByte, size64_t size, uintptr_t buffer){
        uint64_t LBAFirstSector = (firstByte / SectorSizeLBA) + this->partition->FirstLBA;

        bool Check;
        uint64_t sizeWrite = 0;
        uint64_t sizeToWrite = 0;
        uint64_t sectorsToWrite = 0;
        uint64_t sectorsWrite = 0;
        for(int i = 0; i < DivideRoundUp(size, port->BufferSize); i++){           
            sizeToWrite = size - sizeWrite;
            if(sizeToWrite > port->BufferSize){
                sizeToWrite = port->BufferSize;
            }

            sectorsToWrite = DivideRoundUp(sizeToWrite, SectorSizeLBA);
            Check = port->Read(LBAFirstSector + sectorsWrite, sectorsToWrite, port->Buffer);
            if(sizeWrite != 0){
                memcpy(globalPageTableManager[0].GetVirtualAddress(port->Buffer), (uintptr_t)((uint64_t)buffer + sizeWrite), sizeToWrite);
            }else{
                memcpy(globalPageTableManager[0].GetVirtualAddress((uintptr_t)((uint64_t)port->Buffer + firstByte % SectorSizeLBA)), buffer, sizeToWrite);
            }
            Check = port->Write(LBAFirstSector + sectorsWrite, sectorsToWrite, port->Buffer);
                       
            sizeWrite += sizeToWrite;
            sectorsWrite += sectorsToWrite;
        }
        return Check;
    }
}

