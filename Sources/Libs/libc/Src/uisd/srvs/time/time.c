#include <kot/uisd/srvs/time.h>

thread_t srv_time_callback_thread = NULL;
time_t* TimePointer = NULL;
uint64_t* TickPointer = NULL;

void Srv_Time_Initialize(){
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);
    process_t proc = Sys_GetProcess();

    thread_t TimeThreadKeyCallback = NULL;
    Sys_CreateThread(proc, &Srv_Time_Callback, PriviledgeApp, NULL, &TimeThreadKeyCallback);
    srv_time_callback_thread = MakeShareableThreadToProcess(TimeThreadKeyCallback, TimeData->ControllerHeader.Process);
}

void Srv_Time_Callback(KResult Status, struct srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Status = Callback->Handler(Status, Callback, GP0, GP1, GP2, GP3);

    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
        
    Sys_Close(KSUCCESS);
}


/* SetTimePointerKey */
KResult Srv_Time_SetTimePointerKey_Callback(KResult Status, struct srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_time_callback_t* Srv_Time_SetTimePointerKey(time_t** Time, bool IsAwait){
    if(!srv_time_callback_thread) Srv_Time_Initialize();
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);

    thread_t self = Sys_GetThread();

    struct srv_time_callback_t* callback = (struct srv_time_callback_t*)malloc(sizeof(struct srv_time_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Time_SetTimePointerKey_Callback; 

    *Time = GetFreeAlignedSpace(sizeof(time_t));

    kot_key_mem_t TimePointerKey;
    Sys_CreateMemoryField(Sys_GetProcess(), sizeof(time_t), (void**)Time, &TimePointerKey, MemoryFieldTypeShareSpaceRO);

    kot_key_mem_t TimePointerKeyShare;
    Sys_Keyhole_CloneModify(TimePointerKey, &TimePointerKeyShare, NULL, KeyholeFlagPresent, PriviledgeApp);

    struct arguments_t parameters;
    parameters.arg[0] = srv_time_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = TimePointerKeyShare;

    KResult Status = Sys_ExecThread(TimeData->SetTimePointerKey, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }else if(Status != KSUCCESS){
        callback->Status = Status;
    }
    return callback;
}


/* SetTickPointerKey */
KResult Srv_Time_SetTickPointerKey_Callback(KResult Status, struct srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    return Status;
}

struct srv_time_callback_t* Srv_Time_SetTickPointerKey(uint64_t* TimePointer, uint64_t TickPeriod, bool IsAwait){
    if(!srv_time_callback_thread) Srv_Time_Initialize();
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);

    thread_t self = Sys_GetThread();

    struct srv_time_callback_t* callback = (struct srv_time_callback_t*)malloc(sizeof(struct srv_time_callback_t));
    callback->Self = self;
    callback->Data = NULL;
    callback->Size = NULL;
    callback->IsAwait = IsAwait;
    callback->Status = KBUSY;
    callback->Handler = &Srv_Time_SetTickPointerKey_Callback; 

    kot_key_mem_t TickPointerKey;
    Sys_CreateMemoryField(Sys_GetProcess(), sizeof(uint64_t), TimePointer, &TickPointerKey, MemoryFieldTypeShareSpaceRO);

    kot_key_mem_t TickPointerKeyShare;
    Sys_Keyhole_CloneModify(TickPointerKey, &TickPointerKeyShare, NULL, KeyholeFlagPresent, PriviledgeApp);

    struct arguments_t parameters;
    parameters.arg[0] = srv_time_callback_thread;
    parameters.arg[1] = callback;
    parameters.arg[2] = TickPointerKeyShare;
    parameters.arg[3] = TickPeriod;

    KResult Status = Sys_ExecThread(TimeData->SetTickPointerKey, &parameters, ExecutionTypeQueu, NULL);
    if(Status == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }else if(Status != KSUCCESS){
        callback->Status = Status;
    }
    return callback;
}


/* Other functions without sever need */

KResult Get_Time_Initialize(){
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);
    while(!TimeData->TimePointerKey) Sys_Schedule(); // Wait dirver
    
    TimePointer = GetFreeAlignedSpace(sizeof(uint64_t));
    Sys_AcceptMemoryField(Sys_GetProcess(), TimeData->TimePointerKey, &TimePointer);
    return KSUCCESS;
}

KResult Get_Tick_Initialize(){
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);
    while(!TimeData->TickPointerKey) Sys_Schedule(); // Wait driver

    TickPointer = GetFreeAlignedSpace(sizeof(uint64_t));
    Sys_AcceptMemoryField(Sys_GetProcess(), TimeData->TickPointerKey, &TickPointer);
    return KSUCCESS;
}

uint64_t GetYear(){
    if(!TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return TimePointer->Year;
}

uint64_t GetMonth(){
    if(!TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return TimePointer->Month;
}

uint64_t GetDay(){
    if(!TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return TimePointer->Day;
}

uint64_t GetWeekDay(){
    if(!TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return TimePointer->WeekDay;
}

uint64_t GetHour(){
    if(!TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return TimePointer->Hour;
}

uint64_t GetMinute(){
    if(!TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return TimePointer->Minute;
}

uint64_t GetSecond(){
    if(!TimePointer){
        KResult status = Get_Time_Initialize();
        if(status != KSUCCESS){
            return NULL;
        }
    }
    return TimePointer->Second;
}

KResult Sleep(uint64_t duration){ // duration in ms
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);

    if(!TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    uint64_t Target = *TickPointer + (FEMOSECOND_IN_MILLISECOND * duration) / TimeData->TickPeriod;
    while(*TickPointer < Target){
        Sys_Schedule();
    }

    return KSUCCESS;
}

KResult SleepFromTick(uint64_t* tick, uint64_t duration){ // duration in ms
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);

    if(!TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    uint64_t Target = *tick + (FEMOSECOND_IN_MILLISECOND * duration) / TimeData->TickPeriod;
    while(*TickPointer < Target){
        Sys_Schedule();
    }

    *tick = Target;
    return KSUCCESS;
}

KResult GetActualTick(uint64_t* tick){
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);

    if(!TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    *tick = *TickPointer;
    return KSUCCESS;
}

KResult GetTimeFromTick(uint64_t* time, uint64_t tick){
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);

    if(!TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    *time = (tick * TimeData->TickPeriod) / FEMOSECOND_IN_MILLISECOND;
    return KSUCCESS;
}

KResult GetFemosecondTimeFromTick(uint64_t* time, uint64_t tick){
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);

    if(!TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }

    *time = tick * TimeData->TickPeriod;
    return KSUCCESS;
}

KResult GetTickFromTime(uint64_t* tick, uint64_t time){
    uisd_time_t* TimeData = (uisd_time_t*)FindControllerUISD(ControllerTypeEnum_Time);

    if(!TickPointer){
        KResult status = Get_Tick_Initialize();
        if(status != KSUCCESS){
            return status;
        }
    }
        
    *tick = (FEMOSECOND_IN_MILLISECOND * time) / TimeData->TickPeriod;
    return KSUCCESS;
}

KResult GetTime(uint64_t* time){
    uint64_t tick;
    GetActualTick(&tick);
    GetTimeFromTick(time, tick);
    return KSUCCESS;
}

KResult CompareTime(uint64_t* compare, uint64_t time0, uint64_t time1){
    *compare = time1 - time0;
    return KSUCCESS;
}

uint64_t GetPosixTime(){
    // TODO
    return NULL;
}