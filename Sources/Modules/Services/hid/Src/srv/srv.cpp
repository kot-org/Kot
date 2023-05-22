#include <srv/srv.h>

uisd_hid_t* SrvData;

KResult InitialiseServer(){
    kot_process_t proc = Sys_GetProcess();

    void* address = GetFreeAlignedSpace(sizeof(uisd_hid_t));
    kot_key_mem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_hid_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_hid_t*)address;
    memset(SrvData, 0, sizeof(uisd_hid_t)); // Clear data

    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = HID_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Hid;
    SrvData->ControllerHeader.Process = kot_ShareProcessKey(proc);

    CreateKeyboardContext(SrvData);
    CreateMouseContext(SrvData);

    CreateControllerUISD(ControllerTypeEnum_Hid, key, true);
    return KSUCCESS;
}