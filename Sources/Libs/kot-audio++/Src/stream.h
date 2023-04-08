#ifndef _AUDIOPP_COMPONENT_H_
#define _AUDIOPP_COMPONENT_H_

#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/types.h>
#include <kot/stdio.h>
#include <kot/uisd/srvs/audio.h>

#include <kot++/vector.h>

namespace Audio{

    struct Buffer{
        memory_buffer_t Buffer;
        uint64_t Index;
    };

    class Stream{
        public:
            Stream(uint64_t OutputID);
            ~Stream();
            srv_audio_device_info_t* GetDeviceInfoStream();
            KResult SetVolume(uint8_t Volume);
            KResult OnDeviceUpdate(uint64_t DeviceID, uint64_t OldDeviceID);
            template <typename T> KResult MixAudioBuffers();
            KResult OnOffsetUpdate();
            KResult AddBuffer(uintptr_t Base, size64_t Size);
        private:
            uint64_t Lock;
            uint64_t OutputID;
            audio_share_buffer_t* StreamBuffer;
            memory_buffer_t LocalStreamBuffer;
            thread_t DeviceUpdate;
            thread_t OffsetUpdate;
            std::vector<Buffer> InputStreams;
    };

}

#endif