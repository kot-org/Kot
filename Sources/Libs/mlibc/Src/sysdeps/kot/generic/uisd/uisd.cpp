#include <stdlib.h>
#include <string.h>
#include <kot/uisd.h>
#include <kot/memory.h>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>

extern "C" {

void* kot_ControllerList[ControllerCount];

size64_t kot_ControllerTypeSize[ControllerCount] = {
    sizeof(kot_uisd_system_t),
    sizeof(kot_uisd_time_t),
    sizeof(kot_uisd_hid_t),
    sizeof(kot_uisd_graphics_t),
    sizeof(kot_uisd_storage_t),
    sizeof(kot_uisd_audio_t),
    sizeof(kot_uisd_usb_t),
    sizeof(kot_uisd_pci_t),
    sizeof(kot_uisd_shell_t)
};

kot_thread_t kot_CallBackUISDThread = NULL;
kot_process_t kot_ProcessKeyForUISD = NULL;

KResult kot_CallbackUISD(uint64_t Task, KResult Status, kot_uisd_callbackInfo_t* Info, uint64_t GP0, uint64_t GP1);

KResult kot_InitializeUISD(){
    kot_thread_t UISDthreadKeyCallback;
    uint64_t UISDKeyFlags = NULL;

    kot_process_t Proc = kot_Sys_GetProcess();

    kot_Sys_CreateThread(Proc, (void*)&kot_CallbackUISD, PriviledgeApp, NULL, &UISDthreadKeyCallback);
    kot_CallBackUISDThread = kot_MakeShareableThreadToProcess(UISDthreadKeyCallback, KotSpecificData.UISDHandlerProcess);

    kot_Sys_Keyhole_CloneModify(Proc, &kot_ProcessKeyForUISD, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent | KeyholeFlagDataTypeProcessMemoryAccessible, PriviledgeApp);
    
    memset(&kot_ControllerList, NULL, ControllerCount * sizeof(void*));
    
    return KSUCCESS;
}

KResult kot_CallbackUISD(uint64_t Task, KResult Status, kot_uisd_callbackInfo_t* Info, uint64_t GP0, uint64_t GP1){
    if(Task == UISDGetTask){
        kot_ControllerList[Info->Controller] = (void*)GP0;
        Info->Location = GP0;
    } 
    Info->Status = Status;
    if(Info->AwaitCallback){
        kot_Sys_Unpause(Info->Self);
    }
    kot_Sys_Close(KSUCCESS);
}

kot_uisd_callbackInfo_t* kot_GetControllerUISD(enum kot_uisd_controller_type_enum Controller, void** Location, bool AwaitCallback){
    if(!kot_CallBackUISDThread) kot_InitializeUISD();
    kot_thread_t Self = kot_Sys_GetThread();
    kot_uisd_callbackInfo_t* Info = (kot_uisd_callbackInfo_t*)malloc(sizeof(kot_uisd_callbackInfo_t));
    Info->Self = Self;
    Info->Controller = Controller;
    Info->AwaitCallback = AwaitCallback;
    Info->Location = NULL;
    Info->Status = KBUSY;

    struct kot_arguments_t parameters;
    parameters.arg[0] = UISDGetTask;
    parameters.arg[1] = Controller;
    parameters.arg[2] = kot_CallBackUISDThread;
    parameters.arg[3] = (uint64_t)Info;
    parameters.arg[4] = kot_ProcessKeyForUISD;
    parameters.arg[5] = (uint64_t)*Location;
    KResult Status = kot_Sys_ExecThread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && AwaitCallback){
        kot_Sys_Pause(false);
        *Location = (void*)Info->Location;
        return Info;
    }
    return Info;
}

kot_uisd_callbackInfo_t* kot_CreateControllerUISD(enum kot_uisd_controller_type_enum Controller, kot_key_mem_t MemoryField, bool AwaitCallback){
    if(!kot_CallBackUISDThread) kot_InitializeUISD();
    kot_thread_t Self = kot_Sys_GetThread();
    kot_uisd_callbackInfo_t* Info = (kot_uisd_callbackInfo_t*)malloc(sizeof(kot_uisd_callbackInfo_t));
    Info->Self = Self;
    Info->Controller = Controller;
    Info->AwaitCallback = AwaitCallback;
    Info->Status = KBUSY;

    kot_key_mem_t MemoryFieldKey = NULL;
    kot_Sys_Keyhole_CloneModify(MemoryField, &MemoryFieldKey, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent, PriviledgeApp);

    struct kot_arguments_t parameters;
    parameters.arg[0] = UISDCreateTask;
    parameters.arg[1] = Controller;
    parameters.arg[2] = kot_CallBackUISDThread;
    parameters.arg[3] = (uint64_t)Info;
    parameters.arg[4] = MemoryFieldKey;
    KResult Status = kot_Sys_ExecThread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && AwaitCallback){
        kot_Sys_Pause(false);
        return Info;
    }
    return Info;
}

/* Useful functions */

kot_thread_t kot_MakeShareableThread(kot_thread_t Thread, enum kot_Priviledge priviledgeRequired){
    kot_thread_t ReturnValue;
    kot_Sys_Keyhole_CloneModify(Thread, &ReturnValue, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
    return ReturnValue;
}

kot_thread_t kot_MakeShareableThreadUISDOnly(kot_thread_t Thread){
    kot_thread_t ReturnValue;
    kot_Sys_Keyhole_CloneModify(Thread, &ReturnValue, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
    return ReturnValue;
}

kot_thread_t kot_MakeShareableThreadToProcess(kot_thread_t Thread, kot_process_t Process){
    kot_thread_t ReturnValue;
    kot_Sys_Keyhole_CloneModify(Thread, &ReturnValue, Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
    return ReturnValue;
}

kot_thread_t kot_MakeShareableSpreadThreadToProcess(kot_thread_t Thread, kot_process_t Process){
    kot_thread_t ReturnValue;
    kot_Sys_Keyhole_CloneModify(Thread, &ReturnValue, Process, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
    return ReturnValue;
}

kot_process_t kot_ShareProcessKey(kot_process_t Process){
    kot_process_t ReturnValue;
    kot_Sys_Keyhole_CloneModify(Process, &ReturnValue, NULL, KeyholeFlagPresent, PriviledgeApp);
    return ReturnValue;
}

void* kot_GetControllerLocationUISD(enum kot_uisd_controller_type_enum Controller){
    return kot_ControllerList[Controller];
}

void* kot_FindControllerUISD(enum kot_uisd_controller_type_enum Controller){
    void* ControllerData = kot_GetControllerLocationUISD(Controller);
    if(!ControllerData){
        ControllerData = kot_GetFreeAlignedSpace(kot_ControllerTypeSize[Controller]);
        kot_uisd_callbackInfo_t* Info = kot_GetControllerUISD(Controller, &ControllerData, true);
        free(Info);
    }
    return ControllerData;
}

KResult kot_ResetUISDThreads(){
    kot_CallBackUISDThread = NULL;

    // Reset every clients
    kot_srv_system_callback_thread = NULL;
    kot_srv_storage_callback_thread = NULL;
    kot_srv_time_callback_thread = NULL;
    kot_srv_pci_callback_thread = NULL;
    // HID doesn't have thread
    kot_srv_graphics_callback_thread = NULL;
    // Authorization doesn't have thread
    kot_srv_audio_callback_thread = NULL;
    
    return KSUCCESS;
}

}

