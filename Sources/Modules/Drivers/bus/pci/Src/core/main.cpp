#include <core/main.h>

extern "C" int main(int argc, char* argv[]) {
    Printlog("[PCI] Initialization ...");

    srv_system_callback_t* Callback = Srv_System_GetTableInRootSystemDescription("MCFG", true);
    uintptr_t MCFGTable = Callback->Data;
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

    Printlog("[PCI] Driver initialized successfully");

    return KSUCCESS;
}

/* List functions */
PCIDeviceListInfo_t* InitPCIList(){
    PCIDeviceListInfo_t* ReturnValue = (PCIDeviceListInfo_t*)malloc(sizeof(PCIDeviceListInfo_t));
    ReturnValue->Devices = new std::Stack(NULL);
    ReturnValue->DevicesNum = NULL;
    return ReturnValue;
}

void AddPCIDevice(PCIDeviceListInfo_t* Devices, PCIDevice_t* Device){
    Devices->Devices->push64((uint64_t)Device);
    Devices->DevicesNum++;
}

void ConvertListToArray(PCIDeviceListInfo_t* DevicesList, PCIDeviceArrayInfo_t* DevicesArray){
    DevicesArray->DevicesNum = 1; // 0 is invalid index
    DevicesArray->Devices = (PCIDevice_t**)malloc(DevicesList->DevicesNum * sizeof(PCIDevice_t*));
    for(size64_t i = 0; i < DevicesList->DevicesNum; i++){
        DevicesArray->Devices[DevicesArray->DevicesNum] = (PCIDevice_t*)DevicesList->Devices->pop64();
        DevicesArray->DevicesNum++;
    }
}


/* Devices function */

bool CheckDevice(PCIDeviceArrayInfo_t* DevicesArray, PCIDeviceID_t device){
    if(DevicesArray->DevicesNum != NULL && device < DevicesArray->DevicesNum){
        return true;
    }
    return false;
}

PCIDevice_t* GetDevice(PCIDeviceArrayInfo_t* DevicesArray, PCIDeviceID_t device){
    return DevicesArray->Devices[device];
}

uint64_t Search(PCIDeviceArrayInfo_t* DevicesArray, uint16_t vendorID, uint16_t deviceID, uint16_t classID, uint16_t subClassID, uint16_t progIF){
    uint8_t checkRequired = 0;
    uint32_t deviceNum = 0;

    if(vendorID != 0xFFFF)
        checkRequired++;
    if(deviceID != 0xFFFF)
        checkRequired++;
    if(subClassID != 0xFFFF)
        checkRequired++;
    if(classID != 0xFFFF)
        checkRequired++;
    if(progIF != 0xFFFF)
        checkRequired++;

    for(uint32_t i = 1; i < DevicesArray->DevicesNum; i++) {

        PCIDeviceHeader_t* header = (PCIDeviceHeader_t*)DevicesArray->Devices[i];

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

PCIDeviceID_t GetDevice(PCIDeviceArrayInfo_t* DevicesArray, uint16_t vendorID, uint16_t deviceID, uint16_t classID, uint16_t subClassID, uint16_t progIF, uint64_t index){
    uint8_t checkRequired = 0;
    uint32_t deviceNum = 0;

    if(vendorID != 0xFFFF)
        checkRequired++;
    if(deviceID != 0xFFFF)
        checkRequired++;
    if(subClassID != 0xFFFF)
        checkRequired++;
    if(classID != 0xFFFF)
        checkRequired++;
    if(progIF != 0xFFFF)
        checkRequired++;

    for(uint32_t i = 1; i < DevicesArray->DevicesNum; i++) {
        
        PCIDeviceHeader_t* header = (PCIDeviceHeader_t*)DevicesArray->Devices[i];

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

        if(index == deviceNum){
            return i;
        }

    }
    return NULL;
}


/* Device function */

uint8_t GetBarType(uint32_t Value){
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

uintptr_t PCIDevice_t::GetBarAddress(uint8_t index){
    PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)ConfigurationSpace;
    switch (Header->HeaderType){
        case 0x0:{
            if(index < 6){
                PCIHeader0_t* Header0 = (PCIHeader0_t*)Header;
                switch (GetBarType(Header0->BAR[index])){
                    case PCI_BAR_TYPE_IO:
                        return (uintptr_t)(Header0->BAR[index] & 0xFFFFFFFC);
                    case PCI_BAR_TYPE_32:
                        return (uintptr_t)(Header0->BAR[index] & 0xFFFFFFF0);
                    case PCI_BAR_TYPE_64:
                        return (uintptr_t)((Header0->BAR[index] & 0xFFFFFFF0) | ((Header0->BAR[index + 1] & 0xFFFFFFFF) << 32));
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

size64_t PCIDevice_t::GetBarSize(uintptr_t addresslow){
    uint32_t BARValue = *(uint32_t*)addresslow;
    uint8_t Type = GetBarType(BARValue);

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
            uintptr_t addresslhigh = (uintptr_t)((uint64_t)addresslow + 0x4);
            /* Write into bar high */
            *(uint32_t*)addresslhigh = 0xFFFFFFFF;
            SendConfigurationSpace();

            /* Read bar high */
            ReceiveConfigurationSpace();
            SizeHigh = *(uint32_t*)addresslhigh;            
        }

        uint64_t Size = SizeLow | (SizeHigh << 32);
        return (~Size + 1);
    }

    return NULL;
}

size64_t PCIDevice_t::GetBarSize(uint8_t index){
    PCIDeviceHeader_t* Header = (PCIDeviceHeader_t*)ConfigurationSpace;
    switch (Header->HeaderType){
        case 0x0:{
            if(index < 6){
                PCIHeader0_t* Header0 = (PCIHeader0_t*)Header;
                return GetBarSize(&Header0->BAR[index]);
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
    switch (Header->HeaderType){
        case 0x0:{
            if(index < 6){
                PCIHeader0_t* Header0 = (PCIHeader0_t*)Header;
                return GetBarType(Header0->BAR[index]);
            }
            break;
        }    
        default:
            break;
    }
    return NULL;
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