#ifndef _AUDIOPP_COMPONENT_H_
#define _AUDIOPP_COMPONENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <kot/sys.h>
#include <kot/types.h>
#include <kot/uisd/srvs/audio.h>

#include <kot++/new.h>
#include <kot++/vector.h>

namespace Audio{

    struct Buffer{
        kot_memory_buffer_t Buffer;
        uint64_t Index;
    };

    class Stream{
        public:
            Stream(uint64_t OutputID);
            ~Stream();
            kot_srv_audio_device_info_t* GetDeviceInfoStream();
            KResult SetVolume(uint8_t Volume);
            KResult OnDeviceUpdate(uint64_t DeviceID, uint64_t OldDeviceID);
            template <typename T> KResult MixAudioBuffers();
            KResult OnOffsetUpdate();
            KResult AddBuffer(void* Base, size64_t Size);
        private:
            uint64_t Lock;
            uint64_t OutputID;
            kot_audio_share_buffer_t* StreamBuffer;
            kot_memory_buffer_t LocalStreamBuffer;
            kot_thread_t DeviceUpdate;
            kot_thread_t OffsetUpdate;
            std::vector<Buffer> InputStreams;
    };

}

#endif