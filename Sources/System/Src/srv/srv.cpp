#include <srv/srv.h>

void InitializeSrv(){
    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_system_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_system_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    uisd_system_t* SystemSrv = (uisd_system_t*)address;
    SystemSrv->ControllerHeader.IsReadWrite = false;
    SystemSrv->ControllerHeader.Version = System_Srv_Version;
    SystemSrv->ControllerHeader.VendorID = Kot_VendorID;
    SystemSrv->ControllerHeader.Type = ControllerTypeEnum_System;

    CreateControllerUISD(ControllerTypeEnum_System, key, true);
}