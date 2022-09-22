#include <srv/srv.h>

uisd_hid_t* SrvData;

KResult InitialiseServer(){
    process_t proc = Sys_GetProcess();

    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_hid_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_hid_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_hid_t*)address;
    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = HID_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Hid;
    SrvData->ControllerHeader.Process = ShareProcessKey(proc);

    CreateKeyboardContext(SrvData);
    CreateMouseContext(SrvData);

    CreateControllerUISD(ControllerTypeEnum_Hid, key, true);
    return KSUCCESS;
}