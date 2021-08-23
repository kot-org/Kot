#include "../../fileSystem/gpt/gpt.h"
#include "../../fileSystem/kfs/kfs.h"
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
        /*Creat static buffer for the disk */
        Buffer = globalAllocator.RequestPage();
        BufferSize = 0x1000;

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

        HBACommandHeader* cmdHeader = (HBACommandHeader*)HbaPort->CommandListBase;
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 0; //read mode
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->CommandTableBaseAddress);
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

        HBACommandHeader* cmdHeader = (HBACommandHeader*)HbaPort->CommandListBase;
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 1; //write mode
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->CommandTableBaseAddress);
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
        HBACommandHeader* cmdHeader = (HBACommandHeader*)HbaPort->CommandListBase;
        cmdHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->Write = 0;
        cmdHeader->PrdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->CommandTableBaseAddress);
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
        DiskInfo = (ATACommandIdentify*)malloc(sizeof(ATACommandIdentify));
        memcpy(DiskInfo, Buffer, sizeof(ATACommandIdentify));
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
        memset(Buffer, 0, BufferSize);
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
        globalLogs->Warning("AHCIDriver is loading"); 
        this->PCIBaseAddress = pciBaseAddress;

        ABAR = (HBAMemory*)((PCI::PCIHeader0*)pciBaseAddress)->BAR5;

        globalPageTableManager.MapMemory(ABAR, ABAR);
        ProbePorts();
        for (int i = 0; i < PortCount; i++){
            Port* port = Ports[i];


            port->Configure();
            GPT::Partitons* Partitons = GPT::GetAllPartitions(port);           

            if(port->PortNumber == 0){
                GPT::GPTHeader* GptHeader = GPT::GetGPTHeader(port);               
                if(!port->IsPortInit(GPT::GetReservedGUIDPartitionType())){   
                    globalLogs->Warning("[AHCI] Disk at port %u not initialize yet", port->PortNumber); 
                    port->ResetDisk();
                    GPT::InitGPTHeader(port);
                    
                    globalLogs->Successful("%x", GPT::GetFreeSizePatition(port)); 
                    GPT::CreatPartition(port, 0, "KotReserved", GPT::GetReservedGUIDPartitionType(), 8);
                    
                    GPT::CreatPartition(port, GPT::GetFreeSizePatition(port), "KotData", GPT::GetDataGUIDPartitionType(), 7);

        
                }
  
                GPT::Partition partitionTest = GPT::Partition(port, GPT::GetPartitionByGUID(port, GPT::GetDataGUIDPartitionType()));  
                FileSystem::KFS* Fs = new FileSystem::KFS(&partitionTest);
                //Fs->flist("");
                FileSystem::File* file = Fs->fopen("terre.txt", "r");
                char* bufferfile = (char*)malloc(4331);
                bufferfile = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque in luctus elit. Ut eleifend posuere imperdiet. Duis quis ante laoreet, blandit erat in, maximus tortor. Nunc sit amet semper massa. Nulla ipsum ex, sollicitudin ac efficitur at, efficitur quis lacus. Sed eros justo, vehicula et suscipit nec, malesuada id velit. Morbi lacinia nulla eros, vel tempor erat blandit in. Proin molestie lectus nisi, vel ultrices turpis posuere et. Morbi nec semper felis. Nullam pretium ipsum mollis, volutpat libero ac, ullamcorper lacus. Nulla imperdiet, sem vel scelerisque lacinia, arcu nulla congue lorem, id tempor est justo sit amet arcu. Integer et congue neque. Cras ultricies lorem eget erat porta pulvinar. Nullam rutrum imperdiet pretium. Ut lobortis sapien non dui pulvinar commodo. Aenean commodo condimentum est at ultrices. Nulla eget magna sed magna imperdiet tempor ut non purus. Maecenas ultricies efficitur libero sit amet interdum. Phasellus egestas, urna in efficitur accumsan, enim tellus aliquam metus, vel sodales velit mauris eget est. Pellentesque posuere nisl est, vel semper orci imperdiet quis. Duis sit amet laoreet tellus. Aenean malesuada lobortis mauris, id dignissim diam commodo at. Nunc ornare dictum sagittis. Donec ut tellus nisl. Nunc pulvinar tempus lacus sit amet sollicitudin.Aliquam convallis urna auctor magna fringilla, quis consequat elit ornare. Duis id ullamcorper tortor. Duis in quam vitae nisl porta dapibus. Sed sit amet ultrices sapien. Quisque consequat, elit sit amet facilisis feugiat, ex lacus aliquam tortor, at condimentum orci neque et neque. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Morbi ut imperdiet magna, eget aliquet leo. Suspendisse sed viverra erat, vitae cursus massa. Morbi dictum consectetur lacus at pharetra. Nam ut porttitor ligula, tincidunt ultrices metus. In placerat a neque non tristique. Ut feugiat tellus sed felis rhoncus volutpat. Suspendisse commodo scelerisque ex ut finibus. Nam neque ante, tincidunt ut ligula vitae, dictum tincidunt nisi. Sed pulvinar, sapien consectetur fringilla fermentum, massa nulla malesuada ex, quis tristique erat sapien sit amet quam. Sed et felis sed odio elementum posuere eget ut lacus.Suspendisse a dictum nulla, in mollis nunc. Fusce lobortis sapien vel lacus varius, et tincidunt lorem facilisis. Mauris dignissim molestie congue. Nunc in imperdiet est, non consectetur nisl. Sed elementum sit amet elit at porttitor. Mauris quis ornare sem. Vivamus egestas venenatis massa quis lobortis. Aliquam semper vulputate metus, a volutpat felis sodales non. Praesent fringilla commodo dolor, at porta urna laoreet faucibus. In a felis sollicitudin, facilisis risus vitae, sodales felis. Mauris ultrices auctor erat, at laoreet sem laoreet auctor. Nunc malesuada non nunc quis dictum. Vestibulum posuere justo velit, eget elementum odio finibus at.Fusce at dignissim augue, ac blandit enim. Aliquam eget sollicitudin libero, ac mattis quam. Nulla at dolor sit amet purus ultricies elementum. Suspendisse ornare, orci et commodo hendrerit, lorem eros vulputate felis, eget ultrices nisl ex eu eros. Praesent bibendum aliquam nisl, quis mattis nibh imperdiet nec. Maecenas semper nibh nisl, nec tristique ipsum pretium tempus. Pellentesque id bibendum ipsum, semper molestie quam. Donec sed diam lorem. Pellentesque lobortis diam non interdum convallis. Quisque eros metus, ullamcorper a sem sit amet, rutrum tincidunt purus. Pellentesque ut augue risus. Proin efficitur pharetra massa a ultricies.Maecenas rutrum lacus diam, nec tempus sem aliquam ut. Donec in nulla elit. Integer eu sem semper, finibus ipsum et, vestibulum dui. Pellentesque vel porttitor odio. Nunc tempor sem ut magna maximus, a rutrum lacus mattis. Cras aliquet orci at nibh mollis iaculis id sit amet augue. Nam congue ligula a libero varius feugiat. Proin lectus risus, convallis at massa eu, tristique placerat lacus.Nulla tempus pellentesque feugiat. Quisque non lorem mollis, consectetur ligula in, bibendum sem. Sed sed magna et dolor pharetra venenatis quis quis est. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Suspendisse rutrum sapien et tristique consequat. Nam fermentum sapien sit amet volutpat luctus. Donec bibendum viverra dui hendrerit posuere.";
                file->Write(0, 4331, bufferfile);
                memset(bufferfile, 0, 4331);
                file->Read(0, 4331, bufferfile);
                for(int i = 0; i < 4331; i++) {
                    globalCOM1->Write(*(uint8_t*)((uint64_t)bufferfile + i));
                }
                /*void* bufferfile = malloc(255);
                memset(bufferfile, 'a', 255);
                file->Write(0, 255, bufferfile);
                memset(bufferfile, 0, 255);
                file->Read(0, 255, bufferfile);
                for(int i = 0; i < 255; i++) {
                    globalCOM1->Write(*(uint8_t*)((uint64_t)bufferfile + i));
                }*/
                //Fs->flist("");
            }      
        }
    }

    AHCIDriver::~AHCIDriver(){
 
    }
}