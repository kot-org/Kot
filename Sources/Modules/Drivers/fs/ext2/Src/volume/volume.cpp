#include <volume/volume.h>

thread_t ListenerEventThread;

GUID_t GUIDToListen[]{
    {.Data0 = 0x0FC63DAF, .Data1 = 0x8483, .Data2 = 0x4772, .Data3 = 0x8E79, .Data4 = 0x3D69D8477DE4}
};

process_t ProcessKey;

void InitializeVolumeListener(){
    ProcessKey = ShareProcessKey(Sys_GetProcess());
    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&ListenerEvent, PriviledgeDriver, NULL, &ListenerEventThread);
    process_t ShareProcess = ShareProcessKey(Sys_GetProcess());
    for(uint64_t i = 0; i < GUIDToListenSize; i++){
        Srv_Storage_NotifyOnNewPartitionByGUIDType(&GUIDToListen[i], ListenerEventThread, ShareProcess, true);
    }
}

void ListenerEvent(uint64_t VolumeID, srv_storage_space_info_t* StorageSpace, process_t VFSProcess, thread_t VFSMountThread){
    srv_storage_device_t* StorageDevice;
    Srv_StorageInitializeDeviceAccess(StorageSpace, &StorageDevice);
    mount_info_t* MountInfo = InitializeMount(StorageDevice);
    MountToVFS(MountInfo, VFSProcess, VFSMountThread);
    Sys_Close(KSUCCESS);
}