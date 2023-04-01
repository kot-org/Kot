#ifndef _SRV_HID_H
#define _SRV_HID_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef KResult (*AudioCallbackHandler)(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

enum AudioDeviceType{
    AudioDeviceTypeOut = 0,
    AudioDeviceTypeIn = 1,
};

enum AudioEncoding{
    AudioEncodingPCMS8LE = 0,
    AudioEncodingPCMS16LE = 1,
    AudioEncodingPCMS20LE = 2,
    AudioEncodingPCMS24LE = 3,
    AudioEncodingPCMS32LE = 4,
};

enum AudioSetStatus{
    AudioSetStatusRunningState = 0,
    AudioSetStatusVolume = 1,
};

typedef struct {
    uint8_t NumberOfChannels;
    enum AudioEncoding Encoding;
    uint64_t SampleRate;
} audio_format;

typedef struct {
    char Name[128];

    enum AudioDeviceType Type;
    audio_format Format;

    thread_t ChangeStatus;

    event_t OnOffsetUpdate;
    size64_t SizeOffsetUpdateToTrigger;

    ksmem_t StreamBufferKey;
    size64_t StreamSize;
    size64_t StreamRealSize;
    size64_t PositionOfStreamData;
} srv_audio_device_t;

typedef uint8_t audio_volume_t;

typedef struct{
    cyclic_t Buffer;
    audio_format Format;
    audio_volume_t Volume; 
}audio_buffer_t;

struct srv_audio_callback_t{
    thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    AudioCallbackHandler Handler;
};

void Srv_Audio_Initialize();

void Srv_Audio_Callback(KResult Status, struct srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_audio_callback_t* Srv_Audio_AddDevice(srv_audio_device_t* Device, bool IsAwait);



#if defined(__cplusplus)
}
#endif


#endif