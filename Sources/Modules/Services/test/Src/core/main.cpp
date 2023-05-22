#include <core/main.h>

void GetMemory(){
    kot_Printlog("[Test] Call another function");
    kot_key_mem_t MemoryShare = NULL;
    void* address = GetFreeAlignedSpace(0x1000);
    kot_key_mem_t key = NULL;
    kot_process_t proc = Sys_GetProcess();
    Sys_CreateMemoryField(proc, 0x1000, &address, &key, MemoryFieldTypeShareSpaceRO);
    char* testchar = "Share from another process\0";
    memcpy(address, testchar, strlen(testchar) + 1);
    kot_key_mem_t KeyShare = NULL;
    Sys_Keyhole_CloneModify(key, &KeyShare, NULL, KeyholeFlagPresent, PriviledgeApp);
    kot_Sys_Close(KeyShare);
}

extern "C" int main(int argc, char* argv[]){
    kot_Printlog("[Test] Hello world");

    kot_process_t proc = Sys_GetProcess();

    kot_thread_t GetMemoryThread = NULL;
    kot_Sys_CreateThread(proc, (void*)&GetMemory, PriviledgeApp, NULL, &GetMemoryThread);

    void* address = GetFreeAlignedSpace(sizeof(uisd_test_t));
    kot_key_mem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_test_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    uisd_test_t* TestSrv = (uisd_test_t*)address;
    TestSrv->ControllerHeader.IsReadWrite = false;
    TestSrv->ControllerHeader.Version = Test_Srv_Version;
    TestSrv->ControllerHeader.VendorID = Kot_VendorID;
    TestSrv->ControllerHeader.Type = ControllerTypeEnum_Test;

    TestSrv->GetMemory = kot_MakeShareableThread(GetMemoryThread, PriviledgeApp);

    CreateControllerUISD(ControllerTypeEnum_Test, key, true);

    kot_Printlog("[Test] End");
}