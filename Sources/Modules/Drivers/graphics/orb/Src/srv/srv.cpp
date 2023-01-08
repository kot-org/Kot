#include <core/main.h>

uisd_graphics_t* SrvData;

KResult InitialiseServer(){
    process_t proc = Sys_GetProcess();

    uintptr_t address = GetFreeAlignedSpace(sizeof(uisd_graphics_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_graphics_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    SrvData = (uisd_graphics_t*)address;
    SrvData->ControllerHeader.IsReadWrite = false;
    SrvData->ControllerHeader.Version = ORB_Srv_Version;
    SrvData->ControllerHeader.VendorID = Kot_VendorID;
    SrvData->ControllerHeader.Type = ControllerTypeEnum_Graphics;
    SrvData->ControllerHeader.Process = ShareProcessKey(proc);

    uisd_callbackInfo_t* Callback = CreateControllerUISD(ControllerTypeEnum_Graphics, key, true);
    KResult Status = Callback->Status;
    free(Callback);
    return Status;
}

KResult NewWindow(thread_t Callback, uint64_t CallbackArg, process_t Target, thread_t Event, uint64_t WindowType){
    KResult Status = KFAIL;
    Window* window = NULL;
    if((window = new Window(WindowType)) != NULL){
        Status = KSUCCESS;
    }
    
    arguments_t arguments{
        .arg[0] = Status,            /* Status */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}
