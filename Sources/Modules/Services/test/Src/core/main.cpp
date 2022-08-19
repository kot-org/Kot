#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Test] Hello world");
    process_t proc = NULL;
    Sys_GetProcessKey(&proc);
    uintptr_t address = getFreeAlihnedSpace(sizeof(graphics_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(graphics_t), &address, &key, MemoryFieldTypeShareSpaceRW);
    CreateControllerUISD(ControllerTypeEnum_Graphics, key, true);
    Printlog("[Test] UISD response");
}