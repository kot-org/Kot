#include <core/main.h>

extern "C" int main(int argc, char* argv[]){
    Printlog("[Test1] Hello world");

    uintptr_t addressReceive = getFreeAlignedSpace(sizeof(uisd_test_t));
    GetControllerUISD(ControllerTypeEnum_Test, &addressReceive, true);

    uisd_test_t* TestSrv = (uisd_test_t*)addressReceive;

    arguments_t arguments;
    Sys_Execthread(TestSrv->GetMemory, &arguments, ExecutionTypeQueu, NULL);

    Printlog("[Test1] End");
}