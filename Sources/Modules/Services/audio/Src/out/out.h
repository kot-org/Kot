#pragma once

#include <core/main.h>

KResult InitializeOutputSrv();

struct OutputDevice_t{
    srv_audio_device_t Device;
    bool StreamIsRunning;
    audio_buffer_t OutputStream;
    std::vector<audio_buffer_t> InputStreams;
    thread_t OnOffsetUpdateHandler;
};

class Outputs{
    private:
        uint64_t Lock;
        std::vector<OutputDevice_t*> Devices;
    public:
        Outputs();    
        KResult AddOutputDevice(srv_audio_device_t* Device);
};