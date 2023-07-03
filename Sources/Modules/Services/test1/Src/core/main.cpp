#include <core/main.h>

int main(int argc, char* argv[]){
    kot_Printlog("[Test1] Hello world");

    kot_process_t proc = Sys_GetProcess();

    void* addressReceive = GetFreeAlignedSpace(sizeof(uisd_test_t));
    GetControllerUISD(ControllerTypeEnum_Test, &addressReceive, true);

    uisd_test_t* TestSrv = (uisd_test_t*)addressReceive;

    kot_arguments_t arguments;
    kot_key_mem_t MemoryShare = kot_Sys_ExecThread(TestSrv->GetMemory, &arguments, ExecutionTypeQueu | ExecutionTypeAwait, NULL);
    void* addressReceiveShare = GetFreeAlignedSpace(0x1000);
    Sys_AcceptMemoryField(proc, MemoryShare, &addressReceiveShare);
    kot_Printlog((char*)addressReceiveShare);

    kot_Printlog("[Test1] End");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}