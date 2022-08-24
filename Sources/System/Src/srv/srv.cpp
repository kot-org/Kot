#include <srv/srv.h>

void InitializeSrv(){
    uintptr_t address = getFreeAlignedSpace(sizeof(system_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(system_t), &address, &key, MemoryFieldTypeShareSpaceRO);
    CreateControllerUISD(ControllerTypeEnum_System, key, true);
}