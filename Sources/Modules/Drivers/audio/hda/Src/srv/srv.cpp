#include <srv/srv.h>

void InitializeSrv(HDAOutput* Output){
    process_t ProcessToShareData = ((uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio))->ControllerHeader.Process;

    thread_t ChangeStatusThread;
    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&ChangeStatus, PriviledgeDriver, (uint64_t)Output, &ChangeStatusThread);
    Output->AudioDevice.ChangeStatus = MakeShareableThreadToProcess(ChangeStatusThread, ProcessToShareData);

    Sys_Event_Create(&Output->OffsetUpdateEvent);
    Sys_Keyhole_CloneModify(Output->OffsetUpdateEvent, &Output->AudioDevice.OffsetUpdate, ProcessToShareData, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable, PriviledgeApp);
    
    Sys_Keyhole_CloneModify(Output->Stream->BufferKey, &Output->AudioDevice.Buffer, ProcessToShareData, KeyholeFlagPresent, PriviledgeApp);
    Output->AudioDevice.BufferSize = Output->Stream->Size;
}

KResult ChangeStatus(thread_t Callback, uint64_t CallbackArg, enum AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2){
    KResult Status = KFAIL;

    HDAOutput* Output = (HDAOutput*)Sys_GetExternalDataThread();

    switch (Function){
        case AudioSetStatusRunningState:{
            Output->ControllerParent->ChangeStatus(Output, GP0);
            break;
        }
        case AudioSetStatusVolume:{
            Output->ControllerParent->SetVolume(Output, GP0 & 0xff);
            break;
        }
        default:{
            break;
        }
    }

    arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    Sys_Close(KSUCCESS);
}