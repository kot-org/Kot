#include <stdlib.h>
#include <string.h>
#include <kot/uisd.h>
#include <kot/memory.h>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>

namespace Kot{
    uintptr_t ControllerList[ControllerCount];

    size64_t ControllerTypeSize[ControllerCount] = {
        sizeof(uisd_system_t),
        sizeof(uisd_time_t),
        sizeof(uisd_hid_t),
        sizeof(uisd_graphics_t),
        sizeof(uisd_storage_t),
        sizeof(uisd_audio_t),
        sizeof(uisd_usb_t),
        sizeof(uisd_pci_t)
    };

    kot_thread_t CallBackUISDThread = NULL;
    kot_process_t ProcessKeyForUISD = NULL;

    KResult CallbackUISD(uint64_t Task, KResult Status, uisd_callbackInfo_t* Info, uint64_t GP0, uint64_t GP1);

    KResult InitializeUISD(){
        kot_thread_t UISDthreadKeyCallback;
        uint64_t UISDKeyFlags = NULL;

        kot_process_t Proc = Sys_GetProcess();

        Sys_CreateThread(Proc, (uintptr_t)&CallbackUISD, PriviledgeApp, NULL, &UISDthreadKeyCallback);
        InitializeThread(UISDthreadKeyCallback);
        CallBackUISDThread = MakeShareableThreadToProcess(UISDthreadKeyCallback, KotSpecificData.UISDHandlerProcess);

        Sys_Keyhole_CloneModify(Proc, &ProcessKeyForUISD, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent | KeyholeFlagDataTypeProcessMemoryAccessible, PriviledgeApp);
        
        memset(&ControllerList, NULL, ControllerCount * sizeof(uintptr_t));
        
        return KSUCCESS;
    }

    KResult CallbackUISD(uint64_t Task, KResult Status, uisd_callbackInfo_t* Info, uint64_t GP0, uint64_t GP1){
        if(Task == UISDGetTask){
            ControllerList[Info->Controller] = (uintptr_t)GP0;
            Info->Location = GP0;
        } 
        Info->Status = Status;
        if(Info->AwaitCallback){
            Sys_Unpause(Info->Self);
        }
        Sys_Close(KSUCCESS);
    }

    uisd_callbackInfo_t* GetControllerUISD(enum ControllerTypeEnum Controller, uintptr_t* Location, bool AwaitCallback){
        if(!CallBackUISDThread) InitializeUISD();
        kot_thread_t Self = Sys_GetThread();
        uisd_callbackInfo_t* Info = (uisd_callbackInfo_t*)malloc(sizeof(uisd_callbackInfo_t));
        Info->Self = Self;
        Info->Controller = Controller;
        Info->AwaitCallback = AwaitCallback;
        Info->Location = NULL;
        Info->Status = KBUSY;

        struct kot_arguments_t parameters;
        parameters.arg[0] = UISDGetTask;
        parameters.arg[1] = Controller;
        parameters.arg[2] = CallBackUISDThread;
        parameters.arg[3] = (uint64_t)Info;
        parameters.arg[4] = ProcessKeyForUISD;
        parameters.arg[5] = (uint64_t)*Location;
        KResult Status = Sys_ExecThread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && AwaitCallback){
            Sys_Pause(false);
            *Location = Info->Location;
            return Info;
        }
        return Info;
    }

    uisd_callbackInfo_t* CreateControllerUISD(enum ControllerTypeEnum Controller, kot_ksmem_t MemoryField, bool AwaitCallback){
        if(!CallBackUISDThread) InitializeUISD();
        kot_thread_t Self = Sys_GetThread();
        uisd_callbackInfo_t* Info = (uisd_callbackInfo_t*)malloc(sizeof(uisd_callbackInfo_t));
        Info->Self = Self;
        Info->Controller = Controller;
        Info->AwaitCallback = AwaitCallback;
        Info->Status = KBUSY;

        kot_ksmem_t MemoryFieldKey = NULL;
        Sys_Keyhole_CloneModify(MemoryField, &MemoryFieldKey, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent, PriviledgeApp);

        struct kot_arguments_t parameters;
        parameters.arg[0] = UISDCreateTask;
        parameters.arg[1] = Controller;
        parameters.arg[2] = CallBackUISDThread;
        parameters.arg[3] = (uint64_t)Info;
        parameters.arg[4] = MemoryFieldKey;
        KResult Status = Sys_ExecThread(KotSpecificData.UISDHandler, &parameters, ExecutionTypeQueu, NULL);
        if(Status == KSUCCESS && AwaitCallback){
            Sys_Pause(false);
            return Info;
        }
        return Info;
    }

    /* Useful functions */

    kot_thread_t MakeShareableThread(kot_thread_t Thread, enum Priviledge priviledgeRequired){
        kot_thread_t ReturnValue;
        Sys_Keyhole_CloneModify(Thread, &ReturnValue, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
        return ReturnValue;
    }

    kot_thread_t MakeShareableThreadUISDOnly(kot_thread_t Thread){
        kot_thread_t ReturnValue;
        Sys_Keyhole_CloneModify(Thread, &ReturnValue, KotSpecificData.UISDHandlerProcess, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
        return ReturnValue;
    }

    kot_thread_t MakeShareableThreadToProcess(kot_thread_t Thread, kot_process_t Process){
        kot_thread_t ReturnValue;
        Sys_Keyhole_CloneModify(Thread, &ReturnValue, Process, KeyholeFlagPresent | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
        return ReturnValue;
    }

    kot_thread_t MakeShareableSpreadThreadToProcess(kot_thread_t Thread, kot_process_t Process){
        kot_thread_t ReturnValue;
        Sys_Keyhole_CloneModify(Thread, &ReturnValue, Process, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagDataTypeThreadIsExecutableWithQueue, PriviledgeApp);
        return ReturnValue;
    }

    kot_process_t ShareProcessKey(kot_process_t Process){
        kot_process_t ReturnValue;
        Sys_Keyhole_CloneModify(Process, &ReturnValue, NULL, KeyholeFlagPresent, PriviledgeApp);
        return ReturnValue;
    }

    uintptr_t GetControllerLocationUISD(enum ControllerTypeEnum Controller){
        return ControllerList[Controller];
    }

    uintptr_t FindControllerUISD(enum ControllerTypeEnum Controller){
        uintptr_t ControllerData = GetControllerLocationUISD(Controller);
        if(!ControllerData){
            ControllerData = GetFreeAlignedSpace(ControllerTypeSize[Controller]);
            uisd_callbackInfo_t* Info = GetControllerUISD(Controller, &ControllerData, true);
            free(Info);
        }
        return ControllerData;
    }
}

