#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <kot/thread.h>

#include <kot/sys.h>
#include <kot/uisd/srvs/system.h>

extern "C" {

KResult kot_launch(const char *path, char *const argv[], char *const envp[]){
    kot_srv_system_callback_t* Callback = kot_Srv_System_LoadExecutable(PriviledgeApp, (char*)path, true);

    if(Callback->Status == KSUCCESS){
        void* MainStackData;
        size64_t SizeMainStackData;

        int argc = 0;
        for(; argv[argc]; argc++);

        kot_SetupStack(&MainStackData, &SizeMainStackData, argc, (char**)argv, (char**)envp);

        kot_ShareDataWithArguments_t Data{
            .Data = MainStackData,
            .Size = SizeMainStackData,
            .ParameterPosition = 0x0,
        };
        kot_arguments_t Parameters;
        Parameters.arg[2] = 0;

        assert(kot_Sys_ExecThread((kot_thread_t)Callback->Data, &Parameters, ExecutionTypeQueu, &Data) == KSUCCESS);
        free(MainStackData);
        free(Callback);
        return KSUCCESS;
    }else{
        free(Callback);
        return KFAIL;
    }
}

}