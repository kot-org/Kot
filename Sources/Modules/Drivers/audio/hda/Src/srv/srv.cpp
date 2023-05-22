#include <srv/srv.h>

KResult InitializeSrv(HDAOutput* Output){
    process_t ProcessToShareData = ((uisd_audio_t*)FindControllerUISD(ControllerTypeEnum_Audio))->ControllerHeader.Process;

    thread_t ChangeStatusThread;
    kot_Sys_CreateThread(Sys_GetProcess(), (void*)&ChangeStatus, PriviledgeDriver, (uint64_t)Output, &ChangeStatusThread);
    Output->AudioDevice.ChangeStatus = MakeShareableThreadToProcess(ChangeStatusThread, ProcessToShareData);

    Sys_Event_Create(&Output->OffsetUpdateEvent);
    Sys_Keyhole_CloneModify(Output->OffsetUpdateEvent, &Output->AudioDevice.OnOffsetUpdate, ProcessToShareData, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable, PriviledgeApp);
    Output->AudioDevice.Info.SizeOffsetUpdateToTrigger = Output->Stream->SizeIOCToTrigger;
    
    Sys_Keyhole_CloneModify(Output->Stream->BufferKey, &Output->AudioDevice.StreamBufferKey, ProcessToShareData, KeyholeFlagPresent, PriviledgeApp);
    Output->AudioDevice.Info.StreamSize = Output->Stream->Size;
    Output->AudioDevice.Info.PositionOfStreamData = Output->Stream->PositionOfStreamData;
    Output->AudioDevice.Info.StreamRealSize = Output->Stream->RealSize;

    switch(Output->Function->Configuration.DefaultDevice){
        case AC_JACK_LINE_OUT:{
            strcpy((char*)&Output->AudioDevice.Info.Name, "HDA line out\0");
            break;
        }
        case AC_JACK_SPEAKER:{
            strcpy((char*)&Output->AudioDevice.Info.Name, "HDA speaker\0");
            break;
        }
        case AC_JACK_TELEPHONY:{
            strcpy((char*)&Output->AudioDevice.Info.Name, "HDA telephony\0");
            break;
        }
        default:{
            strcpy((char*)&Output->AudioDevice.Info.Name, "HDA output\0");
            break;
        }
    }

    return KSUCCESS;
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

    kot_arguments_t arguments{
        .arg[0] = Status,               /* Status */
        .arg[1] = CallbackArg,          /* CallbackArg */
        .arg[2] = NULL,                 /* GP0 */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_Sys_ExecThread(Callback, &arguments, ExecutionTypeQueu, NULL);
    kot_Sys_Close(KSUCCESS);
}