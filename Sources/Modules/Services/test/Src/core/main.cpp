#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Test] Hello world");
    process_t proc = NULL;
    Sys_GetProcessKey(&proc);
    uintptr_t address = getFreeAlihnedSpace(sizeof(graphics_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(graphics_t), &address, &key, MemoryFieldTypeShareSpaceRW);
    CreateControllerUISD(ControllerTypeEnum_Graphics, key, true);

    uintptr_t addressReceive = getFreeAlihnedSpace(sizeof(graphics_t));
    GetControllerUISD(ControllerTypeEnum_Graphics, &addressReceive, true);

    char* test = "test ipc";
    memcpy(address, test, strlen(test));

    Printlog((char*)address);
    Printlog((char*)addressReceive);
    Printlog("[Test] UISD response");
}