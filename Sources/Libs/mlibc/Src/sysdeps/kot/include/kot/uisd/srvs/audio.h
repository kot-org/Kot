#ifndef KOT_SRV_AUDIO_H
#define KOT_SRV_AUDIO_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/atomic.h>
#include <kot/memory.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define AUDIO_STREAM_CLOSE          0x0
#define AUDIO_STREAM_SET_VOLUME     0x1

typedef KResult (*kot_AudioCallbackHandler)(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

enum kot_AudioDeviceType{
    AudioDeviceTypeOut = 0,
    AudioDeviceTypeIn = 1,
};

enum kot_AudioEncoding{
    AudioEncodingPCMS8LE = 0,
    AudioEncodingPCMS16LE = 1,
    AudioEncodingPCMS20LE = 2,
    AudioEncodingPCMS24LE = 3,
    AudioEncodingPCMS32LE = 4,
};

enum kot_AudioSetStatus{
    AudioSetStatusRunningState = 0,
    AudioSetStatusVolume = 1,
};

typedef struct {
    uint8_t NumberOfChannels;
    enum kot_AudioEncoding Encoding;
    uint64_t SampleRate;
} kot_audio_format;


typedef struct {
    char Name[128];

    bool IsDefault;
    enum kot_AudioDeviceType Type;
    kot_audio_format Format;

    size64_t SizeOffsetUpdateToTrigger;

    size64_t StreamSize;
    size64_t StreamRealSize;
    size64_t PositionOfStreamData;
} kot_srv_audio_device_info_t;

typedef struct {
    kot_srv_audio_device_info_t Info;

    kot_thread_t ChangeStatus;

    kot_event_t OnOffsetUpdate;

    kot_ksmem_t StreamBufferKey;
} kot_srv_audio_device_t;

typedef uint8_t kot_audio_volume_t;

typedef struct{
    kot_cyclic_t Buffer;
    kot_audio_format Format;
    kot_audio_volume_t Volume; 
} kot_audio_buffer_t;

typedef struct{
    kot_ksmem_t StreamBufferKey;
    size64_t StreamSize;
    size64_t StreamRealSize;
    size64_t PositionOfStreamData;
    kot_audio_format Format;
    
    kot_event_t OnOffsetUpdate;
    size64_t SizeOffsetUpdateToTrigger;

    kot_thread_t StreamCommand;
} kot_audio_share_buffer_t;

struct kot_srv_audio_callback_t{
    kot_thread_t Self;
    uint64_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    kot_AudioCallbackHandler Handler;
};

void kot_Srv_Audio_Initialize();

void kot_Srv_Audio_Callback(KResult Status, struct kot_srv_audio_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct kot_srv_audio_callback_t* kot_Srv_Audio_RequestStream(uint64_t OutputID, bool IsAwait);
struct kot_srv_audio_callback_t* kot_Srv_Audio_StreamCommand(kot_audio_share_buffer_t* ShareBuffer, uint64_t Command, uint64_t GP0, uint64_t GP1, uint64_t GP2, bool IsAwait);

KResult kot_CloseStream(kot_audio_share_buffer_t* ShareBuffer);
KResult kot_ChangeVolumeStream(kot_audio_share_buffer_t* ShareBuffer, uint8_t Volume);

struct kot_srv_audio_callback_t* kot_Srv_Audio_ChangeVolume(uint64_t OutputID, uint8_t Volume, bool IsAwait);
struct kot_srv_audio_callback_t* kot_Srv_Audio_SetDefault(uint64_t OutputID, bool IsAwait);
struct kot_srv_audio_callback_t* kot_Srv_Audio_GetDeviceInfo(uint64_t OutputID, bool IsAwait);

struct kot_srv_audio_callback_t* kot_Srv_Audio_AddDevice(kot_srv_audio_device_t* Device, bool IsAwait);


#if defined(__cplusplus)
} 
#endif

#endif