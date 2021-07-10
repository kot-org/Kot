#include "../../../fileSystem/gpt/gpt.h"
#include "../../../fileSystem/kfs/kfs.h"
#include "ahci.h"


namespace AHCI{

    #define HBA_PORT_DEV_PRESENT 0x3
    #define HBA_PORT_IPM_ACTIVE 0x1
    #define SATA_SIG_ATAPI 0xEB140101
    #define SATA_SIG_ATA 0x00000101
    #define SATA_SIG_SEMB 0xC33C0101
    #define SATA_SIG_PM 0x96690101

    #define HBA_PxCMD_CR 0x8000
    #define HBA_PxCMD_FRE 0x0010
    #define HBA_PxCMD_ST 0x0001
    #define HBA_PxCMD_FR 0x4000

    PortType CheckPortType(HBAPort* port){
        uint32_t sataStatus = port->SataStatus;

        uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
        uint8_t deviceDetection = sataStatus & 0b111;

        if (deviceDetection != HBA_PORT_DEV_PRESENT) return PortType::None;
        if (interfacePowerManagement != HBA_PORT_IPM_ACTIVE) return PortType::None;

        switch (port->Signature){
            case SATA_SIG_ATAPI:
                return PortType::SATAPI;
            case SATA_SIG_ATA:
                return PortType::SATA;
            case SATA_SIG_PM:
                return PortType::PM;
            case SATA_SIG_SEMB:
                return PortType::SEMB;
            default:
                PortType::None;
        }
    }

    void AHCIDriver::ProbePorts(){
        uint32_t portsImplemented = ABAR->PortsImplemented;
        for (int i = 0; i < 32; i++){
            if (portsImplemented & (1 << i)){
                PortType portType = CheckPortType(&ABAR->Ports[i]);

                if (portType == PortType::SATA || portType == PortType::SATAPI){
                    Ports[PortCount] = new Port();
                    Ports[PortCount]->portType = portType;
                    Ports[PortCount]->HbaPort = &ABAR->Ports[i];
                    Ports[PortCount]->PortNumber = PortCount;
                    PortCount++;
                }
            }
        }
    }

    void Port::Configure(){
        StopCMD();

        void* newBase = globalAllocator.RequestPage();
        HbaPort->CommandListBase = (uint32_t)(uint64_t)newBase;
        HbaPort->CommandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
        memset((void*)(HbaPort->CommandListBase), 0, 1024);

        void* fisBase = globalAllocator.RequestPage();
        HbaPort->FisBaseAddress = (uint32_t)(uint64_t)fisBase;
        HbaPort->FisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
        memset(fisBase, 0, 256);

        HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)HbaPort->CommandListBase + ((uint64_t)HbaPort->CommandListBaseUpper << 32));

        for (int i = 0; i < 32; i++){
            cmdHeader[i].PrdtLength = 8;

            void* cmdTableAddress = globalAllocator.RequestPage();
            uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
            cmdHeader[i].CommandTableBaseAddress = (uint32_t)(uint64_t)address;
            cmdHeader[i].CommandTableBaseAddressUpper = (uint32_t)((uint64_t)address >> 32);
            memset(cmdTableAddress, 0, 256);
        }

        StartCMD();

        /* get disk info */
        GetDiskInfo();
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

    bool Port::Read(uint64_t sector, uint16_t sectorCount, void* buffer){ //LBA so the sector size is 512 bytes
        uint32_t sectorL = (uint32_t) sector;
        uint32_t sectorH = (uint32_t) (sector >> 32);

        HbaPort->InterruptStatus = (uint32_t) - 1; // Clear pending interrupt bits

        HBACommandHeader* cmdHeader = (HBACommandHeader*)HbaPort->CommandListBase;
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 0; //read mode
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->CommandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry));

        commandTable->PrdtEntry[0].DataBaseAddress = (uint32_t)(uint64_t)buffer;
        commandTable->PrdtEntry[0].DataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->PrdtEntry[0].ByteCount = (sectorCount << 9) - 1; // 512 bytes per sector
        commandTable->PrdtEntry[0].InterruptOnCompletion = 1;

        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->CommandFIS);

        cmdFIS->FisType = FIS_TYPE_REG_H2D;
        cmdFIS->CommandControl = 1; // command
        cmdFIS->Command = ATA_CMD_READ_DMA_EX; //read command

        cmdFIS->Lba0 = (uint8_t)sectorL;
        cmdFIS->Lba1 = (uint8_t)(sectorL >> 8);
        cmdFIS->Lba2 = (uint8_t)(sectorL >> 16);
        cmdFIS->Lba3 = (uint8_t)sectorH;
        cmdFIS->Lba4 = (uint8_t)(sectorH >> 8);
        cmdFIS->Lba4 = (uint8_t)(sectorH >> 16);

        cmdFIS->DeviceRegister = 1 << 6; //LBA mode

        cmdFIS->CountLow = sectorCount & 0xFF;
        cmdFIS->CountHigh = (sectorCount >> 8) & 0xFF;

        uint64_t spin = 0;
        uint64_t timeOut = 1000000;
        while ((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < timeOut){
            spin ++;
        }
        if (spin >= timeOut) {
            return false;
        }

        HbaPort->CommandIssue = 1;

        while (true){

            if((HbaPort->CommandIssue == 0)) break;
            if(HbaPort->InterruptStatus & HBA_PxIS_TFES)
            {
                return false;
            }
        }

        return true;
    }

    bool Port::Write(uint64_t sector, uint16_t sectorCount, void* buffer){ //LBA so the sector size is 512 bytes
        uint32_t sectorL = (uint32_t) sector;
        uint32_t sectorH = (uint32_t) (sector >> 32);

        HbaPort->InterruptStatus = (uint32_t) - 1; // Clear pending interrupt bits

        HBACommandHeader* cmdHeader = (HBACommandHeader*)HbaPort->CommandListBase;
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 1; //write mode
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->CommandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry));

        commandTable->PrdtEntry[0].DataBaseAddress = (uint32_t)(uint64_t)buffer;
        commandTable->PrdtEntry[0].DataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->PrdtEntry[0].ByteCount = (sectorCount << 9) - 1;
        commandTable->PrdtEntry[0].InterruptOnCompletion = 1;

        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->CommandFIS);

        cmdFIS->FisType = FIS_TYPE_REG_H2D;
        cmdFIS->CommandControl = 1; // command
        cmdFIS->Command = ATA_CMD_WRITE_DMA_EX; //write command

        cmdFIS->Lba0 = (uint8_t)sectorL;
        cmdFIS->Lba1 = (uint8_t)(sectorL >> 8);
        cmdFIS->Lba2 = (uint8_t)(sectorL >> 16);
        cmdFIS->Lba3 = (uint8_t)sectorH;
        cmdFIS->Lba4 = (uint8_t)(sectorH >> 8);
        cmdFIS->Lba4 = (uint8_t)(sectorH >> 16);

        cmdFIS->DeviceRegister = 1 << 6; //LBA mode

        cmdFIS->CountLow = sectorCount & 0xFF;
        cmdFIS->CountHigh = (sectorCount >> 8) & 0xFF;

        uint64_t spin = 0;
        uint64_t timeOut = 1000000;
        while ((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < timeOut){
            spin ++;
        }
        if (spin >= timeOut) {
            return false;
        }

        HbaPort->CommandIssue = 1;

        while (true){

            if((HbaPort->CommandIssue == 0)) break;
            if(HbaPort->InterruptStatus & HBA_PxIS_TFES)
            {
                return false;
            }
        }

        return true;
    }
    bool Port::GetDiskInfo(){
        uint16_t* buffer = (uint16_t*)globalAllocator.RequestPage();
        HBACommandHeader* cmdHeader = (HBACommandHeader*)HbaPort->CommandListBase;
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 0;
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->CommandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->PrdtLength-1)*sizeof(HBAPRDTEntry));

        commandTable->PrdtEntry[0].DataBaseAddress = (uint32_t)(uint64_t)buffer;
        commandTable->PrdtEntry[0].DataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->PrdtEntry[0].ByteCount = sizeof(ATACommandIdentify);
        commandTable->PrdtEntry[0].InterruptOnCompletion = 1;

        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->CommandFIS);

        cmdFIS->FisType = FIS_TYPE_REG_H2D;
        cmdFIS->CommandControl = 1; // command
        cmdFIS->Command = ATA_CMD_IDENTIFY; //identify command

        uint64_t spin = 0;
        uint64_t timeOut = 1000000;
        while ((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < timeOut){
            spin ++;
        }
        if (spin >= timeOut) {
            return false;
        }

        HbaPort->CommandIssue = 1; //execute the command

        while (true){
            if((HbaPort->CommandIssue == 0)) break; //verify if the command is finished
            if(HbaPort->InterruptStatus & HBA_PxIS_TFES)
            {
                return false;
            }
        }
        this->DiskInfo = (ATACommandIdentify*)buffer;
        return true;
    }

    uint64_t Port::GetNumberSectorsLBA(){ //this function get the size / 512 to get the number of sectors it only specify as 512 bytes secotrs
        return this->DiskInfo->TotalNumberUserAddressableLBASectorsAvailable; //LBA always specify sectors as 512 
    }

    uint64_t Port::GetSectorNumberPhysical(){
        uint64_t ReturnValue = this->GetSize() / this->GetSectorSizePhysical();
        return ReturnValue;
    }

    uint16_t Port::GetSectorSizeLBA(){
        return 512; //512 is the size of sector in lba
    }

    uint16_t Port::GetSectorSizePhysical(){
        return this->DiskInfo->SectorSize;
    }

    uint64_t Port::GetSize(){
        uint64_t ReturnValue = this->GetNumberSectorsLBA() * GetSectorSizeLBA(); //512 is the size of a sector in lba because we get lba number of sectors
        return ReturnValue;
    }

    uint16_t* Port::GetModelNumber(){       
        return this->DiskInfo->DriveModelNumber;
    }

    uint16_t* Port::GetSerialNumber(){
        return this->DiskInfo->SerialNumber;
    }

    void Port::ResetDisk(){
        uint32_t ResetPerSliceBytes = 4096 * this->GetSectorSizeLBA();
        uint16_t SectorResetCount = ResetPerSliceBytes / this->GetSectorSizeLBA();
        this->Buffer = globalAllocator.RequestPage();
        memset(this->Buffer, 0, ResetPerSliceBytes);
        int i = 0;
        while(true){
            globalGraphics->Clear();
            this->Write(i * SectorResetCount, SectorResetCount, this->Buffer);          
            printf("%u / %u", i * SectorResetCount, this->GetNumberSectorsLBA());
            globalGraphics->Update();
            if((i * SectorResetCount) > this->GetNumberSectorsLBA()){
                break;
            }
            i++;
        }
        globalGraphics->Clear();
    }

    bool Port::IsPortInit(GUID* GUIDOfInitPartition){
        GPT::Partitons* AllPartitions = GPT::GetAllPartitions(this);
        for(int i = 0; i < AllPartitions->NumberPartitionsCreated; i++){
            if(AllPartitions->AllParitions[i]->PartitionTypeGUID.Data1 == GUIDOfInitPartition->Data1 &&
               AllPartitions->AllParitions[i]->PartitionTypeGUID.Data2 == GUIDOfInitPartition->Data2 &&
               AllPartitions->AllParitions[i]->PartitionTypeGUID.Data3 == GUIDOfInitPartition->Data3 &&
               AllPartitions->AllParitions[i]->PartitionTypeGUID.Data4 == GUIDOfInitPartition->Data4){
                   return true;
            }
        } 
        return false;
    }
    
    bool Port::IsPortSystem(GUID* GUIDOfSystemPartition){
        GPT::Partitons* AllPartitions = GPT::GetAllPartitions(this);
        for(int i = 0; i < AllPartitions->NumberPartitionsCreated; i++){
            if(AllPartitions->AllParitions[i]->PartitionTypeGUID.Data1 == GUIDOfSystemPartition->Data1 &&
               AllPartitions->AllParitions[i]->PartitionTypeGUID.Data2 == GUIDOfSystemPartition->Data2 &&
               AllPartitions->AllParitions[i]->PartitionTypeGUID.Data3 == GUIDOfSystemPartition->Data3 &&
               AllPartitions->AllParitions[i]->PartitionTypeGUID.Data4 == GUIDOfSystemPartition->Data4){
                   return true;
            }
        } 
        return false;
    }

    AHCIDriver::AHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress){
        this->PCIBaseAddress = pciBaseAddress;

        ABAR = (HBAMemory*)((PCI::PCIHeader0*)pciBaseAddress)->BAR5;

        globalPageTableManager.MapMemory(ABAR, ABAR);
        ProbePorts();
        for (int i = 0; i < PortCount; i++){
            Port* port = Ports[i];

            port->Configure();    

            GPT::Partitons* Partitons = GPT::GetAllPartitions(port);
            

            if(port->PortNumber == 1){
                GPT::GPTHeader* GptHeader = GPT::GetGPTHeader(port);                
                if(!port->IsPortInit(GPT::GetReservedGUIDPartitionType())){
                    port->ResetDisk();
                    GPT::InitGPTHeader(port);
                    
                    GPT::CreatPartition(port, 0, "KotReserved", GPT::GetReservedGUIDPartitionType(), 8);
                    
                    GPT::CreatPartition(port, GPT::GetFreeSizePatition(port), "KotData", GPT::GetDataGUIDPartitionType(), 7);
        
                }
                GPT::Partition = GPT::Partition(port, GPT::GetPartitionByGUID(port, GPT::GetDataGUIDPartitionType()));
            } 

            Partitons = GPT::GetAllPartitions(port);
            
            GPT::AllPartitionsInfo[i]->Port = port;
            for(int t = 0; t < Partitons->NumberPartitionsCreated; t++){
                GPT::AllPartitionsInfo[GPT::AllPartitionsInfoNumber]->Partition = Partitons->AllParitions[t];
                GPT::AllPartitionsInfoNumber++;
            }                   
        }
    }

    AHCIDriver::~AHCIDriver(){
 
    }
}