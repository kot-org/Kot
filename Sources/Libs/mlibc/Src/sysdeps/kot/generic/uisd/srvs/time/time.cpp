#include <kot/uisd/srvs/time.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

kot_thread_t kot_srv_time_callback_thread = NULL;
kot_time_t* kot_TimePointer = NULL;
uint64_t* kot_TickPointer = NULL;

void kot_Srv_Time_Initialize(){
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);
    kot_process_t proc = kot_Sys_GetProcess();

    kot_thread_t TimeThreadKeyCallback = NULL;
    kot_Sys_CreateThread(proc, (uintptr_t)&kot_Srv_Time_Callback, PriviledgeApp, NULL, &TimeThreadKeyCallback);
    kot_InitializeThread(TimeThreadKeyCallback);
    kot_srv_time_callback_thread = kot_MakeShareableThreadToProcess(TimeThreadKeyCallback, TimeData->ControllerHeader.Process);
}

void kot_Srv_Time_Callback(KResult Status, struct kot_srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        kot_Sys_Unpause(Callback->Self);
    }
        
    kot_Sys_Close(KSUCCESS);
}


/* SetTimePointerKey */
KResult Srv_Time_SetTimePointerKey_Callback(KResult Status, struct kot_srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_time_callback_t* kot_Srv_Time_SetTimePointerKey(kot_time_t** Time, bool IsAwait){
    if(!kot_srv_time_callback_thread) kot_Srv_Time_Initialize();
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_time_callback_t* callback = (struct kot_srv_time_callback_t*)malloc(sizeof(struct kot_srv_time_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Time_SetTimePointerKey_Callback; 

    *Time = (kot_time_t*)kot_GetFreeAlignedSpace(sizeof(kot_time_t));

    kot_key_mem_t TimePointerKey;
    kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), sizeof(kot_time_t), (uintptr_t*)Time, &TimePointerKey, MemoryFieldTypeShareSpaceRO);

    kot_key_mem_t TimePointerKeyShare;
    kot_Sys_Keyhole_CloneModify(TimePointerKey, &TimePointerKeyShare, NULL, KeyholeFlagPresent, PriviledgeApp);

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_time_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = TimePointerKeyShare;

    KResult Status = kot_Sys_ExecThread(TimeData->SetTimePointerKey, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }else if(Status != KSUCCESS){
        callback->Status = Status;
    }
    return callback;
}


/* SetTickPointerKey */
KResult Srv_Time_SetTickPointerKey_Callback(KResult Status, struct kot_srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct kot_srv_time_callback_t* kot_Srv_Time_SetTickPointerKey(uint64_t* TimePointer, uint64_t TickPeriod, bool IsAwait){
    if(!kot_srv_time_callback_thread) kot_Srv_Time_Initialize();
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);

    kot_thread_t self = kot_Sys_GetThread();

    struct kot_srv_time_callback_t* callback = (struct kot_srv_time_callback_t*)malloc(sizeof(struct kot_srv_time_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Time_SetTickPointerKey_Callback; 

    kot_key_mem_t TickPointerKey;
    kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), sizeof(uint64_t), TimePointer, &TickPointerKey, MemoryFieldTypeShareSpaceRO);

    kot_key_mem_t TickPointerKeyShare;
    kot_Sys_Keyhole_CloneModify(TickPointerKey, &TickPointerKeyShare, NULL, KeyholeFlagPresent, PriviledgeApp);

    struct kot_arguments_t parameters;
    parameters.arg[0] = kot_srv_time_callback_thread;
    parameters.arg[1] = (uint64_t)callback;
    parameters.arg[2] = TickPointerKeyShare;
    parameters.arg[3] = TickPeriod;

    KResult Status = kot_Sys_ExecThread(TimeData->SetTickPointerKey, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        kot_Sys_Pause(false);
    }else if(Status != KSUCCESS){
        callback->Status = Status;
    }
    return callback;
}


/* Other functions without sever need */

KResult Get_Time_Initialize(){
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);
    while(!TimeData->TimePointerKey) kot_Sys_Schedule(); // Wait dirver
    
    kot_TimePointer = (kot_time_t*)kot_GetFreeAlignedSpace(sizeof(kot_time_t));
    kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), TimeData->TimePointerKey, (uintptr_t*)&kot_TimePointer);
    return KSUCCESS;
}

KResult Get_Tick_Initialize(){
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);
    while(!TimeData->TickPointerKey) kot_Sys_Schedule(); // Wait driver

    kot_TickPointer = (uint64_t*)kot_GetFreeAlignedSpace(sizeof(uint64_t));
    kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), TimeData->TickPointerKey, (uintptr_t*)&kot_TickPointer);
    return KSUCCESS;
}

uint64_t kot_GetYear(){
    if(!kot_TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return kot_TimePointer->Year;
}

uint64_t kot_GetMonth(){
    if(!kot_TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return kot_TimePointer->Month;
}

uint64_t kot_GetDay(){
    if(!kot_TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return kot_TimePointer->Day;
}

uint64_t kot_GetWeekDay(){
    if(!kot_TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return kot_TimePointer->WeekDay;
}

uint64_t kot_GetHour(){
    if(!kot_TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return kot_TimePointer->Hour;
}

uint64_t kot_GetMinute(){
    if(!kot_TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return kot_TimePointer->Minute;
}

uint64_t kot_GetSecond(){
    if(!kot_TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return kot_TimePointer->Second;
}

KResult kot_Sleep(uint64_t duration){ // duration in ns
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);

    if(!kot_TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    uint64_t Target = *kot_TickPointer + (FEMOSECOND_IN_NANOSECOND * duration) / TimeData->TickPeriod;
    while(*kot_TickPointer < Target){
        kot_Sys_Schedule();
    }

    return KSUCCESS;
}

KResult kot_SleepFromTick(uint64_t* tick, uint64_t duration){ // duration in ns
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);

    if(!kot_TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    uint64_t Target = *tick + (FEMOSECOND_IN_NANOSECOND * duration) / TimeData->TickPeriod;
    while(*kot_TickPointer < Target){
        kot_Sys_Schedule();
    }

    *tick = Target;
    return KSUCCESS;
}

KResult kot_GetActualTick(uint64_t* tick){
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);

    if(!kot_TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    *tick = *kot_TickPointer;
    return KSUCCESS;
}

KResult kot_GetTimeFromTick(uint64_t* time, uint64_t tick){
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);

    if(!kot_TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    *time = (tick * TimeData->TickPeriod) / FEMOSECOND_IN_NANOSECOND;
    return KSUCCESS;
}

KResult kot_GetFemosecondTimeFromTick(uint64_t* time, uint64_t tick){
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);

    if(!kot_TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    *time = tick * TimeData->TickPeriod;
    return KSUCCESS;
}

KResult kot_GetTickFromTime(uint64_t* tick, uint64_t time){
    kot_uisd_time_t* TimeData = (kot_uisd_time_t*)kot_FindControllerUISD(ControllerTypeEnum_Time);

    if(!kot_TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }
        
    *tick = (FEMOSECOND_IN_NANOSECOND * time) / TimeData->TickPeriod;
    return KSUCCESS;
}

KResult kot_GetTime(uint64_t* time){
    uint64_t tick;
    kot_GetActualTick(&tick);
    kot_GetTimeFromTick(time, tick);
    return KSUCCESS;
}

KResult kot_CompareTime(uint64_t* compare, uint64_t time0, uint64_t time1){
    *compare = time1 - time0;
    return KSUCCESS;
}

uint64_t kot_GetPosixTime(){
    // TODO
    return NULL;
}

}