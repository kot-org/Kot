#include <partition/mbr/mbr.h>

KResult device_partitions_t::LoadMBRHeader(){
    if(!MBRHeader){
        MBRHeader = (MBRHeader_t*)malloc(sizeof(MBRHeader_t));
    }
    KResult status = Device->ReadDevice(MBRHeader, NULL, sizeof(MBRHeader_t));
    if(status == KSUCCESS){
        IsMBRHeaderLoaded = true;
    }
    return status;
}

bool device_partitions_t::IsMBRDisk(){
    if(!IsMBRHeaderLoaded){
        if(LoadMBRHeader() != KSUCCESS){
            return KFAIL;
        }
    }
    return (MBRHeader->Signature == MBR_Signature);
}

bool device_partitions_t::IsGPTDisk(){
    if(!IsMBRHeaderLoaded){
        if(LoadMBRHeader() != KSUCCESS){
            return false;
        }
    }

    if(IsMBRDisk()){
        // check if MBR protective
        if(MBRHeader->PartitionRecord[0].OSIndicator == 0xEE){
            return true;
        }
    }
    
    return false;
}