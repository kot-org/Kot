#include <srv/srv.h>

kot_uisd_hid_t* SrvData;

KResult InitialiseServer(){
    kot_process_t proc = kot_Sys_GetProcess();

    void* address = kot_GetFreeAlignedSpace(sizeof(kot_uisd_hid_t));
    kot_key_mem_t key = NULL;
    kot_Sys_CreateMemoryField(proc, sizeof(kot_uisd_hid_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (kot_uisd_hid_t*)address;
    memset(SrvData, 0, sizeof(kot_uisd_hid_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = HID_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Hid;
    SrvData->ControllerHeader.Process = kot_ShareProcessKey(proc);

    CreateKeyboardContext(SrvData);
    CreateMouseContext(SrvData);

    kot_CreateControllerUISD(ControllerTypeEnum_Hid, key, true);
    return KSUCCESS;
}