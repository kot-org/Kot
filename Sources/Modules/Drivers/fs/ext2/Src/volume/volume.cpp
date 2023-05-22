#include <volume/volume.h>

kot_thread_t ListenerEventThread;

GUID_t GUIDToListen[]{
    {.Data0 = 0x0FC63DAF, .Data1 = 0x8483, .Data2 = 0x4772, .Data3 = 0x8E79, .Data4 = 0x3D69D8477DE4}
};

kot_process_t ProcessKey;

void InitializeVolumeListener(){
    ProcessKey = kot_ShareProcessKey(Sys_GetProcess());
    kot_Sys_CreateThread(Sys_GetProcess(), (void*)&ListenerEvent, PriviledgeDriver, NULL, &ListenerEventThread);
    kot_process_t ShareProcess = kot_ShareProcessKey(Sys_GetProcess());
    for(uint64_t i = 0; i < GUIDToListenSize; i++){
        Srv_Storage_NotifyOnNewPartitionByGUIDType(&GUIDToListen[i], ListenerEventThread, ShareProcess, true);
    }
}

void ListenerEvent(uint64_t VolumeID, srv_storage_space_info_t* StorageSpace, kot_process_t VFSProcess, kot_thread_t VFSMountThread){
    srv_storage_device_t* StorageDevice;
    Srv_StorageInitializeDeviceAccess(StorageSpace, &StorageDevice);
    mount_info_t* MountInfo = InitializeMount(StorageDevice);
    MountToVFS(MountInfo, VFSProcess, VFSMountThread);
    kot_Sys_Close(KSUCCESS);
}