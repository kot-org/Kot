#pragma once

#include <core/main.h>

#include <out/out.h>

#define Audio_Srv_Version 0x1

struct CallbackAudio{
    thread_t Self;
    KResult Status;
};

struct AddDeviceExternalData{
    class Outputs* OutputsClass;
    // TODO : add inputs class
};

KResult InitialiseServer();

UISDServerEntry KResult RequestStream(thread_t Callback, uint64_t CallbackArg, uint64_t OutputID, process_t ProcessKey);
UISDServerEntry KResult StreamCommand(thread_t Callback, uint64_t CallbackArg, uint64_t Command, uint64_t GP0, uint64_t GP1, uint64_t GP2);
UISDServerEntry KResult ChangeVolume(thread_t Callback, uint64_t CallbackArg, uint64_t OutputID, uint8_t Volume);
UISDServerEntry KResult SetDefault(thread_t Callback, uint64_t CallbackArg, uint64_t OutputID);
UISDServerEntry KResult GetDeviceCount(thread_t Callback, uint64_t CallbackArg);
UISDServerEntry KResult GetDeviceInfo(thread_t Callback, uint64_t CallbackArg, uint64_t OutputID);

UISDServerEntry KResult AddDevice(thread_t Callback, uint64_t CallbackArg, srv_audio_device_t* Device);

CallbackAudio* ChangeStatus(srv_audio_device_t* Device, enum AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult SetRunningState(srv_audio_device_t* Device, bool IsRunning);
KResult SetVolume(srv_audio_device_t* Device, audio_volume_t Volume);