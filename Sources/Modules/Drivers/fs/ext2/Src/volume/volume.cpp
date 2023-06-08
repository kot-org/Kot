#include <volume/volume.h>

kot_thread_t ListenerEventThread;

kot_GUID_t GUIDToListen[]{
    {.Data0 = 0x0FC63DAF, .Data1 = 0x8483, .Data2 = 0x4772, .Data3 = 0x798E, .Data4{0x3D, 0x69, 0xD8, 0x47, 0x7D, 0xE4}}
};

kot_process_t ProcessKey;

void InitializeVolumeListener(){
    ProcessKey = kot_ShareProcessKey(kot_Sys_GetProcess());
    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&ListenerEvent, PriviledgeDriver, NULL, &ListenerEventThread);
    kot_process_t ShareProcess = kot_ShareProcessKey(kot_Sys_GetProcess());
    for(uint64_t i = 0; i < GUIDToListenSize; i++){
        kot_Srv_Storage_NotifyOnNewPartitionByGUIDType(&GUIDToListen[i], ListenerEventThread, ShareProcess, true);
    }
}

void ListenerEvent(uint64_t VolumeID, kot_srv_storage_space_info_t* StorageSpace, kot_process_t VFSProcess, kot_thread_t VFSMountThread){
    kot_srv_storage_device_t* StorageDevice;
    kot_Srv_StorageInitializeDeviceAccess(StorageSpace, &StorageDevice);
    mount_info_t* MountInfo = InitializeMount(StorageDevice);
    MountToVFS(MountInfo, VFSProcess, VFSMountThread);
    kot_Sys_Close(KSUCCESS);
}