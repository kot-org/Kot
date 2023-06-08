#include <core/main.h>

int main(int argc, char* argv[]) {
    kot_Printlog("[BUS/PCI] Initialization ...");

    kot_srv_system_callback_t* Callback = kot_Srv_System_GetTableInRootSystemDescription("MCFG", true);
    void* MCFGTable = (void*)Callback->Data;
    free(Callback);

    PCIDeviceListInfo_t* PCIDeviceList = InitPCIList();

    if(MCFGTable != NULL){
        InitPCIe(PCIDeviceList, MCFGTable);
    }else{
        InitPCI(PCIDeviceList);
    }

    PCIDeviceArrayInfo_t* PCIDeviceArray = (PCIDeviceArrayInfo_t*)malloc(sizeof(PCIDeviceArrayInfo_t));

    ConvertListToArray(PCIDeviceList, PCIDeviceArray);

    InitSrv(PCIDeviceArray);

    kot_Printlog("[BUS/PCI] Driver initialized successfully");

    return KSUCCESS;
}

/* List functions */
PCIDeviceListInfo_t* InitPCIList(){
    PCIDeviceListInfo_t* ReturnValue = (PCIDeviceListInfo_t*)malloc(sizeof(PCIDeviceListInfo_t));
    ReturnValue->Devices = new std::Stack(0x80);
    ReturnValue->DevicesNum = NULL;
    return ReturnValue;
}

void AddPCIDevice(PCIDeviceListInfo_t* DevicesList, PCIDevice_t* Device){
    // Setup command register
    PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)Device->ConfigurationSpace;
    Device->SendConfigurationSpace();

    // Add device to list
    DevicesList->Devices->push64((uint64_t)Device);
    DevicesList->DevicesNum++;
}

void ConvertListToArray(PCIDeviceListInfo_t* DevicesList, PCIDeviceArrayInfo_t* DevicesArray){
    DevicesArray->DevicesNum = 1; // 0 is invalid index
    DevicesArray->Devices = (PCIDevice_t**)malloc((DevicesList->DevicesNum + 1) * sizeof(PCIDevice_t*));
    for(size64_t i = 0; i < DevicesList->DevicesNum; i++){
        DevicesArray->Devices[DevicesArray->DevicesNum] = (PCIDevice_t*)DevicesList->Devices->pop64();
        DevicesArray->DevicesNum++;
    }
}


/* Devices function */

bool CheckDevice(PCIDeviceArrayInfo_t* DevicesArray, kot_PCIDeviceID_t device){
    if(DevicesArray->DevicesNum != NULL && device < DevicesArray->DevicesNum){
        return true;
    }
    return false;
}

PCIDevice_t* GetDeviceFromIndex(PCIDeviceArrayInfo_t* DevicesArray, kot_PCIDeviceID_t device){
    return DevicesArray->Devices[device];
}

uint64_t Search(PCIDeviceArrayInfo_t* DevicesArray, uint16_t vendorID, uint16_t deviceID, uint16_t classID, uint16_t subClassID, uint16_t progIF){
    uint8_t checkRequired = 0;
    uint32_t deviceNum = 0;

    if(vendorID != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;
    if(deviceID != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;
    if(subClassID != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;
    if(classID != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;
    if(progIF != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;

    for(uint32_t i = 1; i < DevicesArray->DevicesNum; i++){
        PCIDeviceHeader_t* header = (PCIDeviceHeader_t*)DevicesArray->Devices[i]->ConfigurationSpace;

        uint8_t checkNum = 0;
        
        if(header->VendorID == vendorID)
            checkNum++;
        if(header->DeviceID == deviceID)
            checkNum++;
        if(header->Subclass == subClassID)
            checkNum++;
        if(header->Class == classID)
            checkNum++;
        if(header->ProgIF == progIF)
            checkNum++;

        if(checkRequired == checkNum) deviceNum++;
    }
    return deviceNum;
}

kot_PCIDeviceID_t GetDevice(PCIDeviceArrayInfo_t* DevicesArray, uint16_t vendorID, uint16_t deviceID, uint16_t classID, uint16_t subClassID, uint16_t progIF, uint64_t index){
    uint8_t checkRequired = 0;
    uint32_t deviceNum = 0;

    if(vendorID != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;
    if(deviceID != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;
    if(subClassID != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;
    if(classID != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;
    if(progIF != PCI_SEARCH_NO_PARAMETER)
        checkRequired++;

    for(uint32_t i = 1; i < DevicesArray->DevicesNum; i++){
        PCIDeviceHeader_t* header = (PCIDeviceHeader_t*)DevicesArray->Devices[i]->ConfigurationSpace;

        uint8_t checkNum = 0;
        
        if(header->VendorID == vendorID)
            checkNum++;
        if(header->DeviceID == deviceID)
            checkNum++;
        if(header->Subclass == subClassID)
            checkNum++;
        if(header->Class == classID)
            checkNum++;
        if(header->ProgIF == progIF)
            checkNum++;

        if(checkRequired == checkNum){
            if(index == deviceNum){
                return i;
            }
            deviceNum++;
        } 


    }
    return NULL;
}


/* Device function */

uint8_t GetBarTypeWithBARValue(uint32_t Value){
    if(Value & 0b1){ /* i/o */
        return PCI_BAR_TYPE_IO;
    }else{
        if(!(Value & 0b110)){ /* 32bits */
            return PCI_BAR_TYPE_32;
        }else if((Value & 0b110) == 0b110){ /* 64bits */
            return PCI_BAR_TYPE_64;
        }
    }
    return PCI_BAR_TYPE_NULL;
}

void* PCIDevice_t::GetBarAddress(uint8_t index){
    PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)ConfigurationSpace;
    switch (Header->HeaderType & 0x7F){
        case 0x0:{
            if(index < 6){
                PCIHeader0_t* Header0 = (PCIHeader0_t*)Header;
                switch (GetBarTypeWithBARValue(Header0->BAR[index])){
                    case PCI_BAR_TYPE_IO:
                        return (void*)(Header0->BAR[index] & 0xFFFFFFFC);
                    case PCI_BAR_TYPE_32:
                        return (void*)(Header0->BAR[index] & 0xFFFFFFF0);
                    case PCI_BAR_TYPE_64:
                        return (void*)((Header0->BAR[index] & 0xFFFFFFF0) | ((Header0->BAR[index + 1] & 0xFFFFFFFF) << 32));
                    default:
                        break;
                }
            }
            break;
        }    
        default:
            break;
    }
    return NULL;
}

size64_t PCIDevice_t::GetBarSizeWithAddress(void* addresslow){
    uint32_t BARValueLow = *(uint32_t*)addresslow;
    uint8_t Type = GetBarTypeWithBARValue(BARValueLow);

    if(Type != PCI_BAR_TYPE_NULL){
        /* Write into bar low */
        *(uint32_t*)addresslow = 0xFFFFFFFF;
        SendConfigurationSpace();

        /* Read bar low */
        ReceiveConfigurationSpace();
        uint32_t SizeLow = *(uint32_t*)addresslow;

        if(Type == PCI_BAR_TYPE_IO){
            SizeLow &= 0xFFFFFFFC;
        }else{
            SizeLow &= 0xFFFFFFF0;
        }


        uint32_t SizeHigh = 0xFFFFFFFF;

        if(Type == PCI_BAR_TYPE_64){
            void* addresshigh = (void*)((uint64_t)addresslow + 0x4);

            uint32_t BARValueHigh = *(uint32_t*)addresshigh;
            /* Write into bar high */
            *(uint32_t*)addresshigh = 0xFFFFFFFF;
            SendConfigurationSpace();

            /* Read bar high */
            ReceiveConfigurationSpace();
            SizeHigh = *(uint32_t*)addresshigh;

            /* Restore value */
            *(uint32_t*)addresshigh = BARValueHigh;   
            SendConfigurationSpace();    
        }

        /* Restore value */
        *(uint32_t*)addresslow = BARValueLow;
        SendConfigurationSpace();

        uint64_t Size = ((uint64_t)(SizeHigh & 0xFFFFFFFF) << 32) | (SizeLow & 0xFFFFFFFF);
        Size = ~Size + 1;
        return Size;
    }

    return NULL;
}

size64_t PCIDevice_t::GetBarSize(uint8_t index){
    PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)ConfigurationSpace;
    switch (Header->HeaderType & 0x7F){
        case 0x0:{
            if(index < 6){
                PCIHeader0_t* Header0 = (PCIHeader0_t*)Header;
                return GetBarSizeWithAddress(&Header0->BAR[index]);
            }
            break;
        }    
        default:
            break;
    }
    return NULL;
}

uint8_t PCIDevice_t::GetBarType(uint8_t index){
    PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)ConfigurationSpace;
    switch (Header->HeaderType & 0x7F){
        case 0x0:{
            if(index < 6){
                PCIHeader0_t* Header0 = (PCIHeader0_t*)Header;
                return GetBarTypeWithBARValue(Header0->BAR[index]);
            }
            break;
        }    
        default:
            break;
    }
    return NULL;
}

KResult PCIDevice_t::BindMSI(uint8_t IRQVector, uint8_t processor, uint16_t localDeviceVector, uint64_t* version){
    PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)ConfigurationSpace;
    switch (Header->HeaderType & 0x7F){
        case 0x0:{
            PCIHeader0_t* Header0 = (PCIHeader0_t*)Header;
            uint8_t CapabilityOffset = Header0->CapabilitiesPtr;
            PCICapability_t* Capability = (PCICapability_t*)((uint64_t)Header + (uint64_t)CapabilityOffset);
            PCICapability_t* CapabilityMSI = NULL;
            PCICapability_t* CapabilityMSIX = NULL;
            while(CapabilityOffset){
                CapabilityOffset = Capability->CapabilityNext;
                if(Capability->CapabilityID == PCICapabilitiesMSI){
                    CapabilityMSI = Capability;
                }else if(Capability->CapabilityID == PCICapabilitiesMSIX){
                    CapabilityMSIX = Capability;
                    break;
                }
                Capability = (PCICapability_t*)((uint64_t)Header + (uint64_t)CapabilityOffset);
            }
            if(CapabilityMSIX){
                CapabilityMSI->MSIX.Control |= 1 << 15; // enable MsiX
                uint64_t TableAddress = ((uint64_t)kot_MapPhysical(GetBarAddress(CapabilityMSIX->MSIX.BIR), GetBarSize(CapabilityMSIX->MSIX.BIR)) + (uint64_t)CapabilityMSIX->MSIX.TableOffset);
                uint16_t Entries = CapabilityMSI->MSIX.Control & 0x7FF;
                if(Entries <= localDeviceVector){
                    PCIMSIXTable_t* Table = (PCIMSIXTable_t*)(TableAddress + sizeof(PCIMSIXTable_t) * localDeviceVector);
                    Table->Address = 0xFEE00000 | (processor << 12);
                    Table->Data = IRQVector; 
                    Table->Control &= ~(1 << 0); // clear first to unmasked MSI
                    SendConfigurationSpace();
                    *version = PCI_MSIX_VERSION;
                    return KSUCCESS;
                }
            }else if(CapabilityMSI){
                if(CapabilityMSI->MSI.Control & (1 << 7)){ // check if support 64 bits
                    CapabilityMSI->MSI.Address = 0xFEE00000 | (processor << 12);
                    CapabilityMSI->MSI.Data = (IRQVector & 0xff) | (1 << 14) | (1 << 15); 
                    CapabilityMSI->MSI.Control &= ~((0b111 << 4) | (1 << 8)); // set 0 for MME and for MSI MASk
                    CapabilityMSI->MSI.Control |= (1 << 0);                   

                    Header->Command |= PCI_COMMAND_INTERRUPT_DISABLE;
                    SendConfigurationSpace();
                    *version = PCI_MSI_VERSION;
                    return KSUCCESS;
                }
            }
            break;
        }    
        default:
            break;
    }
    return KFAIL;
}

KResult PCIDevice_t::UnbindMSI(uint16_t localDeviceVector){
    PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)ConfigurationSpace;
    switch (Header->HeaderType & 0x7F){
        case 0x0:{
            PCIHeader0_t* Header0 = (PCIHeader0_t*)Header;
            uint8_t CapabilityOffset = Header0->CapabilitiesPtr;
            PCICapability_t* Capability = (PCICapability_t*)((uint64_t)Header + (uint64_t)CapabilityOffset);
            PCICapability_t* CapabilityMSI = NULL;
            PCICapability_t* CapabilityMSIX = NULL;
            while(CapabilityOffset){
                CapabilityOffset = Capability->CapabilityNext;
                if(Capability->CapabilityID == PCICapabilitiesMSI){
                    CapabilityMSI = Capability;
                } else if(Capability->CapabilityID == PCICapabilitiesMSIX){
                    CapabilityMSIX = Capability;
                    break;
                }
                Capability = (PCICapability_t*)((uint64_t)Header + (uint64_t)CapabilityOffset);
            }
            if(CapabilityMSIX){
                CapabilityMSI->MSIX.Control |= 1 << 15; // enable MsiX
                uint64_t TableAddress = ((uint64_t)kot_MapPhysical(GetBarAddress(CapabilityMSIX->MSIX.BIR), GetBarSize(CapabilityMSIX->MSIX.BIR)) + (uint64_t)CapabilityMSIX->MSIX.TableOffset);
                uint16_t Entries = CapabilityMSI->MSIX.Control & 0x7FF;
                if(Entries <= localDeviceVector){
                    PCIMSIXTable_t* Table = (PCIMSIXTable_t*)(TableAddress + sizeof(PCIMSIXTable_t) * localDeviceVector);
                    Table->Control |= 1 << 0; // masked MSI
                    Table->Address = NULL;
                    Table->Data = NULL; 
                    SendConfigurationSpace();
                    return KSUCCESS;
                }
            }else if(CapabilityMSI){
                CapabilityMSI->MSI.Control &= ~(1 << 0);  // disable MSI
                CapabilityMSI->MSI.Address = NULL;
                CapabilityMSI->MSI.Data = NULL; 
                SendConfigurationSpace();
                return KSUCCESS;
            }
            break;
        }    
        default:
            break;
    }
    return KFAIL;
}

/* Conguration space */
KResult PCIDevice_t::ConfigReadWord(uint16_t Offset, uint16_t* Value){
    // Check Offset
    if(IsPCIe){
        if(Offset > (PCI_CONFIGURATION_SPACE_PCIE - sizeof(uint16_t))){
            return KFAIL;
        }
    }else{
        if(Offset > (PCI_CONFIGURATION_SPACE_PCI - sizeof(uint16_t))){
            return KFAIL;
        }
    }
    *Value = *(uint16_t*)((uint64_t)ConfigurationSpace + (uint64_t)Offset);
    ReceiveConfigurationSpace();
    return KSUCCESS;
}

KResult PCIDevice_t::ConfigWriteWord(uint16_t Offset, uint16_t Value){
    // Check Offset
    if(IsPCIe){
        if(Offset > (PCI_CONFIGURATION_SPACE_PCIE - sizeof(uint16_t))){
            return KFAIL;
        }
    }else{
        if(Offset > (PCI_CONFIGURATION_SPACE_PCI - sizeof(uint16_t))){
            return KFAIL;
        }
    }
    *(uint16_t*)((uint64_t)ConfigurationSpace + (uint64_t)Offset) = Value;
    SendConfigurationSpace();
    return KSUCCESS;
}

/* Version specific */

void PCIDevice_t::ReceiveConfigurationSpace(){
    if(IsPCIe){
        ReceiveConfigurationSpacePCIe(this);
    }else{
        ReceiveConfigurationSpacePCI(this);
    }
}

void PCIDevice_t::SendConfigurationSpace(){
    if(IsPCIe){
        ReceiveConfigurationSpacePCIe(this);
    }else{
        ReceiveConfigurationSpacePCI(this);
    }
} 