#include <core/main.h>

void GetMemory(){
    Printlog("[Test] Call another function");
    SYS_Close(KSUCCESS);
}

extern "C" int main(int argc, char* argv[]){
    Printlog("[Test] Hello world");

    process_t proc = NULL;
    Sys_GetProcessKey(&proc);

    thread_t GetMemoryThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetMemory, PriviledgeApp, NULL, &GetMemoryThread);

    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_test_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_test_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    uisd_test_t* TestSrv = (uisd_test_t*)address;
    TestSrv->ControllerHeader.IsReadWrite = false;
    TestSrv->ControllerHeader.Version = Test_Srv_Version;
    TestSrv->ControllerHeader.VendorID = Kot_VendorID;
    TestSrv->ControllerHeader.Type = ControllerTypeEnum_Test;

    uint64_t UISDKeyFlags = NULL;
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&UISDKeyFlags, KeyholeFlagDataTypethreadIsExecutableWithQueue, true);
    Sys_Keyhole_CloneModify(GetMemoryThread, &TestSrv->GetMemory, NULL, UISDKeyFlags, PriviledgeApp);

    CreateControllerUISD(ControllerTypeEnum_Test, key, true);

    Printlog("[Test] End");
}