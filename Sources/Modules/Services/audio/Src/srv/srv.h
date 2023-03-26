#pragma once

#include <core/main.h>

#include <out/out.h>

#define Audio_Srv_Version 0x1

struct CallbackAudio{
    thread_t Self;
    KResult Status;
};

KResult InitialiseServer();

KResult AddDevice(thread_t Callback, uint64_t CallbackArg, srv_audio_device_t* Device);

CallbackAudio* ChangeStatus(srv_audio_device_t* Device, enum AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult SetVolume(srv_audio_device_t* Device, uint8_t Volume);