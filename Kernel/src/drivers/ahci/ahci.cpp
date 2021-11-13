#include "../../fileSystem/gpt/gpt.h"
#include "../../fileSystem/kfs/kfs.h"
#include "ahci.h"


namespace AHCI{
    AHCI::AHCIDriver* ahciDriver;


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
        /*Creat static buffer for the disk */
        Buffer = globalAllocator.RequestPage();
        BufferSize = 0x1000;

        StopCMD();

        void* newBase = globalAllocator.RequestPage();
        HbaPort->CommandListBase = (uint32_t)(uint64_t)newBase;
        HbaPort->CommandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
        memset(globalPageTableManager.GetVirtualAddress((void*)(HbaPort->CommandListBase)), 0, 1024);

        void* fisBase = globalAllocator.RequestPage();
        HbaPort->FisBaseAddress = (uint32_t)(uint64_t)fisBase;
        HbaPort->FisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
        memset(globalPageTableManager.GetVirtualAddress(fisBase), 0, 256);

        HBACommandHeader* cmdHeader = (HBACommandHeader*)globalPageTableManager.GetVirtualAddress((void*)((uint64_t)HbaPort->CommandListBase + ((uint64_t)HbaPort->CommandListBaseUpper << 32)));

        for (int i = 0; i < 32; i++){
            cmdHeader[i].PrdtLength = 8;

            void* cmdTableAddress = globalAllocator.RequestPage();
            uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
            cmdHeader[i].CommandTableBaseAddress = (uint32_t)(uint64_t)address;
            cmdHeader[i].CommandTableBaseAddressUpper = (uint32_t)((uint64_t)address >> 32);
            memset(globalPageTableManager.GetVirtualAddress(cmdTableAddress), 0, 256);
        }

        StartCMD();
        /* get disk info */
        GetDiskInfo();

        globalLogs->Successful("[AHCI] Configure port %u", PortNumber);
        if(DiskInfo->SectorSize == 0){
            globalLogs->Warning("[AHCI] No disk detected at port %u", PortNumber);
        }
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

        HBACommandHeader* cmdHeader = (HBACommandHeader*)globalPageTableManager.GetVirtualAddress((void*)HbaPort->CommandListBase);
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 0; //read mode
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)globalPageTableManager.GetVirtualAddress((void*)cmdHeader->CommandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry));

        commandTable->PrdtEntry[0].DataBaseAddress = (uint64_t)buffer;
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

        HBACommandHeader* cmdHeader = (HBACommandHeader*)globalPageTableManager.GetVirtualAddress((void*)HbaPort->CommandListBase);
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 1; //write mode
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)globalPageTableManager.GetVirtualAddress((void*)cmdHeader->CommandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry));

        commandTable->PrdtEntry[0].DataBaseAddress = (uint64_t)buffer;
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
        HBACommandHeader* cmdHeader = (HBACommandHeader*)globalPageTableManager.GetVirtualAddress((void*)HbaPort->CommandListBase);
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 0;
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)globalPageTableManager.GetVirtualAddress((void*)cmdHeader->CommandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->PrdtLength - 1) * sizeof(HBAPRDTEntry));
        commandTable->PrdtEntry[0].DataBaseAddress = (uint64_t)Buffer;
        commandTable->PrdtEntry[0].ByteCount = sizeof(ATACommandIdentify);
        commandTable->PrdtEntry[0].InterruptOnCompletion = 1;

        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->CommandFIS);

        cmdFIS->FisType = FIS_TYPE_REG_H2D;
        cmdFIS->CommandControl = 1; // command
        cmdFIS->Command = ATA_CMD_IDENTIFY; //identify command

        uint64_t spin = 0;
        uint64_t timeOut = 1000000;
        while ((HbaPort->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < timeOut){
            spin++;
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
        DiskInfo = (ATACommandIdentify*)malloc(sizeof(ATACommandIdentify));
        memcpy(DiskInfo, globalPageTableManager.GetVirtualAddress(Buffer), sizeof(ATACommandIdentify));
        return true;
    }

    uint64_t Port::GetNumberSectorsLBA(){ //this function get the size / 512 to get the number of sectors it only specify as 512 bytes secotrs
        return DiskInfo->TotalNumberUserAddressableLBASectorsAvailable; //LBA always specify sectors as 512 
    }

    uint64_t Port::GetSectorNumberPhysical(){
        uint64_t ReturnValue = GetSize() / GetSectorSizePhysical();
        return ReturnValue;
    }

    uint16_t Port::GetSectorSizeLBA(){
        return 512; //512 is the size of sector in lba
    }

    uint16_t Port::GetSectorSizePhysical(){
        return DiskInfo->SectorSize;
    }

    uint64_t Port::GetSize(){
        uint64_t ReturnValue = GetNumberSectorsLBA() * GetSectorSizeLBA(); //512 is the size of a sector in lba because we get lba number of sectors
        return ReturnValue;
    }

    uint16_t* Port::GetModelNumber(){       
        return DiskInfo->DriveModelNumber;
    }

    uint16_t* Port::GetSerialNumber(){
        return DiskInfo->SerialNumber;
    }

    void Port::ResetDisk(){
        memset(globalPageTableManager.GetVirtualAddress(Buffer), 0, BufferSize);
        uint64_t sectorReset = 0;
        uint64_t sectorToReset = GetNumberSectorsLBA();
        uint64_t sectorResetByWrite = BufferSize / GetSectorSizeLBA();
        globalLogs->Warning("Reset start ... : %u", sectorToReset);
        for(sectorReset; sectorReset < sectorToReset; sectorReset++){
            if(sectorToReset - sectorReset > sectorResetByWrite){
                Write(sectorReset, sectorResetByWrite, Buffer);  
            }else{
                Write(sectorReset, sectorToReset - sectorReset, Buffer); 
            }
        }
    }

    bool Port::IsPortInit(GUID* GUIDOfInitPartition){  
        GPT::Partitions* AllPartitions = GPT::GetAllPartitions(this);
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
        GPT::Partitions* AllPartitions = GPT::GetAllPartitions(this);
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
        PartitionNode* LastNode = NULL;
        PartitionNode* CurrentNode;
        ahciDriver = this;
        globalLogs->Warning("[AHCI] Driver is loading"); 
        this->PCIBaseAddress = pciBaseAddress;

        ABAR = (HBAMemory*)((PCI::PCIHeader0*)pciBaseAddress)->BAR5;
        ABAR = (HBAMemory*)globalPageTableManager.MapMemory((void*)ABAR, 1);

        ProbePorts();
        for (int i = 0; i < PortCount; i++){
            Port* port = Ports[i];


            port->Configure();
            
            GPT::Partitions* Partitons = GPT::GetAllPartitions(port);  
            for(int y = 0; y < Partitons->NumberPartitionsCreated; y++){
                CurrentNode = (PartitionNode*)malloc(sizeof(PartitionNode));
                if(PartitionsList == NULL) PartitionsList = CurrentNode;
                CurrentNode->Content.PartitionInfo = (GUIDPartitionEntryFormat*)Partitons->AllParitions[y];
               
                CurrentNode->Last = LastNode;
                if(LastNode != NULL){
                    LastNode->Next = CurrentNode;
                }
                LastNode = CurrentNode;
            }    

            if(port->PortNumber == 1){
                GPT::GPTHeader* GptHeader = GPT::GetGPTHeader(port);             
                if(!port->IsPortInit(GPT::GetSystemGUIDPartitionType())){   
                    globalLogs->Warning("[AHCI] Disk at port %u not initialize yet", port->PortNumber); 
                    
                    GPT::InitGPTHeader(port);
                
                    GPT::CreatPartition(port, GPT::GetFreeSizePatition(port), "KotData", GPT::GetSystemGUIDPartitionType(), 7);        
                }
            }                 
        }
    }

    AHCIDriver::~AHCIDriver(){
 
    }

    PartitionInfo* AHCIDriver::GetSystemPartition(){
        PartitionNode* list = PartitionsList;
        GUID* guid = GPT::GetSystemGUIDPartitionType();
        while(list != NULL){
            if(list->Content.PartitionInfo->PartitionTypeGUID.Data1 == guid->Data1 &&
                list->Content.PartitionInfo->PartitionTypeGUID.Data2 == guid->Data2 &&
                list->Content.PartitionInfo->PartitionTypeGUID.Data3 == guid->Data3 &&
                list->Content.PartitionInfo->PartitionTypeGUID.Data4 == guid->Data4){
                return &list->Content;
            }
            list = list->Next;
        }
    }
}