#pragma once

#include <unistd.h>

#include <kot/types.h>

#include <kot-audio++/stream.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#define BUFFER_COUNT    0x2
#define BUFFER_SIZE     0x2EE0

struct Metadata_t{
    char* Album;
    char* Artist;
    char* Title;
    char* Description;
};

struct TrackInfo_t{
    int64_t Hours;
    int64_t Mins;
    int64_t Secs;
    float Duration;
    Metadata_t Metadata;
};

struct AudioHandlerBuffer_t{
    void* Buffer;
    bool IsFree;
};

class AudioHandler{
    public:
        AudioHandler();
        ~AudioHandler();

        bool GetPlay();
        void SetPlay(bool Status);

        void SetCurrentTimestamp(float Time);
        float GetCurrentTimestamp();

        TrackInfo_t* GetTrackInfo();

        KResult LoadTrack(char* Path);
        void ClearTrack();

        Audio::Stream* GetStream();
        KResult SetStream(Audio::Stream* St);

        void FreeStreamBuffer(Audio::Stream* St, Audio::Buffer* Buffer);
        void DecodeAudio();

        kot_event_t OnNewFrameEvent;
        kot_arguments_t OnNewFrameEventArgs;
    private:
        KResult ReloadSwr();

        bool IsPlaying = false;
        float Timestamp = 0;
        float TimestampSeek = 0;
        bool IsTimestampChanged = false;
        char* TrackPath = NULL;
        TrackInfo_t TrackInfo{
            .Duration = 0,
            .Metadata{
                .Album = NULL,
                .Artist = NULL,
                .Title = NULL,
            },
        };

        Audio::Stream* Stream = NULL;
        kot_audio_format* Format = NULL;
        size_t SampleSize = 0;
        int64_t LastIndexStreamBuffer = 0;
        

        uint64_t BufferCount = BUFFER_COUNT;
        uint64_t LastBufferAllocated = 0;
        uint64_t LastBufferFree = 0;
        AudioHandlerBuffer_t Buffers[BUFFER_COUNT];
        size_t MaxSamplesInBuffer = 0;

        size_t SamplesWritten = 0;
        size_t SizeWritten = 0;

        int AvStreamIndex = -1;
        AVFormatContext* AvFormatCtx = NULL;
        AVStream* AvStream = NULL;
        AVFrame* AvFrame = NULL;
        AVPacket* AvPacket = NULL;
        AVCodecContext* AvCodecCtx = NULL;
        SwrContext* SwrCtx = NULL;

        kot_thread_t DecodeAudioThread = NULL;

        uint64_t Lock = 0;

        bool IsDecodePaused = false;
};