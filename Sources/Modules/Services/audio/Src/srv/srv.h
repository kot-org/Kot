#pragma once

#include <core/main.h>

#include <out/out.h>

#define Audio_Srv_Version 0x1

struct CallbackAudio{
    kot_thread_t Self;
    KResult Status;
};

struct AddDeviceExternalData{
    class Outputs* OutputsClass;
    // TODO : add inputs class
};

KResult InitialiseServer();

UISDServerEntry KResult RequestStream(kot_thread_t Callback, uint64_t CallbackArg, uint64_t OutputID, kot_process_t ProcessKey);
UISDServerEntry KResult StreamCommand(kot_thread_t Callback, uint64_t CallbackArg, uint64_t Command, uint64_t GP0, uint64_t GP1, uint64_t GP2);
UISDServerEntry KResult ChangeVolume(kot_thread_t Callback, uint64_t CallbackArg, uint64_t OutputID, uint8_t Volume);
UISDServerEntry KResult SetDefault(kot_thread_t Callback, uint64_t CallbackArg, uint64_t OutputID);
UISDServerEntry KResult GetDeviceCount(kot_thread_t Callback, uint64_t CallbackArg);
UISDServerEntry KResult GetDeviceInfo(kot_thread_t Callback, uint64_t CallbackArg, uint64_t OutputID);

UISDServerEntry KResult AddDevice(kot_thread_t Callback, uint64_t CallbackArg, kot_srv_audio_device_t* Device);

CallbackAudio* ChangeStatus(kot_srv_audio_device_t* Device, enum kot_AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult SetRunningState(kot_srv_audio_device_t* Device, bool IsRunning);
KResult SetVolume(kot_srv_audio_device_t* Device, kot_audio_volume_t Volume);