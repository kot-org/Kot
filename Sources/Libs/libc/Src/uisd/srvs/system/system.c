#include <kot/uisd/srvs/system.h>

thread_t srv_system_callback_thread = NULL;
uisd_system_t* SystemData = NULL;

void Srv_System_Initialize(){
    SystemData = (uisd_system_t*)FindControllerUISD(ControllerTypeEnum_System);
    if(SystemData != NULL){
        process_t Proc = Sys_GetProcess();

        thread_t SystemthreadKeyCallback = NULL;
        Sys_Createthread(Proc, &Srv_System_Callback, PriviledgeApp, &SystemthreadKeyCallback);
        srv_system_callback_thread = MakeShareableThreadToProcess(SystemthreadKeyCallback, SystemData->ControllerHeader.Process);
    }else{
        Sys_Close(KFAIL);
    }
}

void Srv_System_Callback(KResult Statu, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    Callback->Statu = Callback->Handler(Statu, Callback, GP0, GP1, GP2, GP3);
    if(Callback->IsAwait){
        Sys_Unpause(Callback->Self);
    }
    Sys_Close(KSUCCESS);
}

/* GetFrameBufer */
KResult Srv_System_GetFrameBuffer_Callback(KResult Statu, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Statu == KSUCCESS){
        memcpy(Callback->Data, (uintptr_t)GP0, sizeof(srv_system_framebuffer_t));
        Callback->Size = (size64_t)sizeof(srv_system_framebuffer_t);
    }
    return Statu;
}

struct srv_system_callback_t* Srv_System_GetFrameBuffer(srv_system_framebuffer_t* framebuffer, bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = framebuffer;
    callback->IsAwait = IsAwait;
    callback->Statu = KBUSY;
    callback->Handler = &Srv_System_GetFrameBuffer_Callback;

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;
    

    KResult statu = Sys_Execthread(SystemData->GetFramebuffer, &parameters, ExecutionTypeQueu, NULL);
    if(statu == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* ReadFile */
KResult Srv_System_ReadFileInitrd_Callback(KResult Statu, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Statu == KSUCCESS){
        srv_system_fileheader_t* FileHeader = (srv_system_fileheader_t*)Callback->Data;
        FileHeader->Data = malloc((size64_t)GP0);
        memcpy(FileHeader->Data, (uintptr_t)GP1, (size64_t)GP0);
        FileHeader->Size = (size64_t)GP0;
    }
    return Statu;
}

struct srv_system_callback_t* Srv_System_ReadFileInitrd(char* Name, srv_system_fileheader_t* file,  bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = (uintptr_t)file;
    callback->Size = sizeof(srv_system_fileheader_t*);
    callback->IsAwait = IsAwait;
    callback->Statu = KBUSY;
    callback->Handler = &Srv_System_ReadFileInitrd_Callback;

    struct ShareDataWithArguments_t data;
    data.Data = Name;
    data.Size = strlen(Name) + 1; // add '\0' char
    data.ParameterPosition = 0x2; 

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;

    KResult statu = Sys_Execthread(SystemData->ReadFileInitrd, &parameters, ExecutionTypeQueu, &data);
    if(statu == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

/* GetTableInRootSystemDescription */
KResult Srv_System_GetTableInRootSystemDescription_Callback(KResult Statu, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3){
    if(Statu == KSUCCESS){
        *(srv_system_sdtheader_t**)Callback->Data = (srv_system_sdtheader_t*)MapPhysical(GP0, sizeof(srv_system_sdtheader_t));
    }
    return Statu;
}

struct srv_system_callback_t* Srv_System_GetTableInRootSystemDescription(char* Name, srv_system_sdtheader_t** SDTHeader, bool IsAwait){
    if(!srv_system_callback_thread) Srv_System_Initialize();
    
    thread_t self = Sys_Getthread();

    struct srv_system_callback_t* callback = (struct srv_system_callback_t*)malloc(sizeof(struct srv_system_callback_t));
    callback->Self = self;
    callback->Data = (uintptr_t)SDTHeader;
    callback->Size = (size64_t)sizeof(srv_system_sdtheader_t*);
    callback->IsAwait = IsAwait;
    callback->Statu = KBUSY;
    callback->Handler = &Srv_System_GetTableInRootSystemDescription_Callback;

    struct ShareDataWithArguments_t data;
    data.Data = Name;
    data.Size = strlen(Name) + 1; // add '\0' char
    data.ParameterPosition = 0x2; 

    struct arguments_t parameters;
    parameters.arg[0] = srv_system_callback_thread;
    parameters.arg[1] = callback;

    KResult statu = Sys_Execthread(SystemData->GetTableInRootSystemDescription, &parameters, ExecutionTypeQueu, &data);
    if(statu == KSUCCESS && IsAwait){
        Sys_Pause(false);
    }
    return callback;
}

