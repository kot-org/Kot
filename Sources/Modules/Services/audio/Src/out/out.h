#pragma once

#include <core/main.h>

KResult InitializeOutputSrv();

struct OutputDevice_t{
    srv_audio_device_t Device;
    bool StreamIsRunning;
    audio_buffer_t OutputStream;
    std::vector<struct StreamRequest_t*> InputStreams;
    thread_t OnOffsetUpdateHandler;
    kot_event_t ClientOnOffsetUpdate;
    kot_event_t ClientOnOffsetUpdateShareableKey;
    uint64_t DeviceID;
    uint64_t Lock;
};

struct StreamRequest_t{
    audio_share_buffer_t* ShareBuffer;
    audio_buffer_t* LocalBuffer;
    uint64_t PID;
    uint64_t OutputID;
    process_t ProcessKey;
    ksmem_t StreamBufferLocalKey;
    uint64_t Index;
    thread_t StreamCommandThread;
    OutputDevice_t* Device;
    class Outputs* Self;
};

class Outputs{
    private:
        uint64_t Lock;
        kot_event_t OnDeviceChanged;
        OutputDevice_t* NullDevice;
        std::vector<OutputDevice_t*> Devices;
        uint64_t DeviceCount;
    public:
        Outputs(kot_event_t OnDeviceChangedEvent);
        KResult AddOutputDevice(srv_audio_device_t* Device);
        StreamRequest_t* RequestStream(uint64_t OutputID, process_t ProcessKey, uint64_t PID);
        KResult CloseStream(StreamRequest_t* Stream);
        KResult ChangeVolume(uint64_t OutputID, uint8_t Volume);
        KResult SetDefault(uint64_t OutputID);
        uint64_t GetDeviceCount();
        KResult GetDeviceInfo(uint64_t OutputID, srv_audio_device_info_t* Info);
};