#include <volume/volume.h>

thread_t ListenerEventThread;

GUID_t GUIDToListen[]{
    {.Data0 = 0x0FC63DAF, .Data1 = 0x8483, .Data2 = 0x4772, .Data3 = 0x8E79, .Data4 = 0x3D69D8477DE4}
};

void InitializeVolumeListener(){
    Sys_Createthread(Sys_GetProcess(), (uintptr_t)&ListenerEvent, PriviledgeDriver, NULL, &ListenerEventThread);
    process_t ShareProcess = ShareProcessKey(Sys_GetProcess());
    for(uint64_t i = 0; i < GUIDToListenSize; i++){
        Srv_Storage_NotifyOnNewPartitionByGUIDType(&GUIDToListen[i], ListenerEventThread, ShareProcess, true);
    }
}

void ListenerEvent(uint64_t VolumeID, srv_storage_space_info_t* StorageSpace){
    Printlog("Ok");
    Sys_Close(KSUCCESS);
}