#include <core/main.h>

void DrawHeader(kui_Context* Ctx){

}

void DrawControls(kui_Context* Ctx){

}

void WindowRenderer(kui_Context* Ctx){
    kui_Container* Cnt;

    kui_begin(Ctx);

    if(kui_begin_window(Ctx, "Audio player", kui_rect(50, 50, 900, 400))){
        Cnt = kui_get_current_container(Ctx);

        DrawHeader(Ctx);

        DrawControls(Ctx);

        kui_end_window(Ctx);
    }
    kui_end(Ctx);
    kui_r_present(Cnt);
}

int BufferLastAllocated = 0;
bool IsBufferFree[2]{false, true};

void FreeStreamBuffer(Audio::Buffer* Buf){
    IsBufferFree[BufferLastAllocated] = true;
    BufferLastAllocated = (BufferLastAllocated + 1) % 2;
}

int main(int argc, char* argv[]){
    char* AudioFile = "/d1:/user/test.mp3";
    char* Title = "Unknow Title";
    char* Artist = "Unknown Artist";
    int AvStreamIndex = -1;
    AVFormatContext* FormatCtx = NULL;
    AVStream* AvStream = NULL;
    AVFrame* Frame = NULL;
    AVPacket* Packet = NULL;
    AVCodecContext* CodecCtx = NULL;
    SwrContext* SwrCtx = NULL;
    kot_thread_t OffsetUpdate = NULL;
    /* Initialize stream */
    Audio::Stream* St = new Audio::Stream(0);

    kot_audio_format* Format = St->GetStreamFormat();
    size_t SampleSize = kot_Srv_Audio_GetEncodingSize(Format->Encoding);

    printf("[Audio-player] Stream -> Channels: %d, Sample Size: %d, Sample Rate: %dHz\n", Format->NumberOfChannels, SampleSize, Format->SampleRate);

    FormatCtx = avformat_alloc_context();

    if(avformat_open_input(&FormatCtx, AudioFile, NULL, NULL)){
        fprintf(stderr, "Failed to open %s\n", AudioFile);
        return 1;
    }

    if(avformat_find_stream_info(FormatCtx, NULL)){
        fprintf(stderr, "Failed to get stream info, potentialy corrupted\n");
        return 1;
    }

    
    /* Get file metadata */
    AVDictionaryEntry* Tag = NULL;
    Tag = av_dict_get(FormatCtx->metadata, "title", Tag, AV_DICT_IGNORE_SUFFIX);
    if(Tag){
        Title = Tag->value;
    }

    Tag = av_dict_get(FormatCtx->metadata, "artist", Tag, AV_DICT_IGNORE_SUFFIX);
    if(Tag){
        Artist = Tag->value;
    }
    uint64_t DurationSeconds = 180;

    AvStreamIndex = av_find_best_stream(FormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    assert(AvStreamIndex >= 0);

    AvStream = FormatCtx->streams[AvStreamIndex];

    Packet = av_packet_alloc();
    const AVCodec* Codec = avcodec_find_decoder(AvStream->codecpar->codec_id);
    if (!Codec) {
        fprintf(stderr, "Failed to find MP3 codec\n");
        return 1;
    }

    CodecCtx = avcodec_alloc_context3(Codec);
    assert(CodecCtx);
    if (avcodec_parameters_to_context(CodecCtx, AvStream->codecpar)) {
        fprintf(stderr, "Failed to init codec context\n");
        return 1;
    }

    if (avcodec_open2(CodecCtx, Codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return 1;
    }

    SwrCtx = swr_alloc();

    av_opt_set_int(SwrCtx, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(SwrCtx, "in_sample_rate", CodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(SwrCtx, "in_sample_fmt", CodecCtx->sample_fmt, 0);

    if(Format->NumberOfChannels == 1){
        av_opt_set_int(SwrCtx, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
    }else if(Format->NumberOfChannels == 2){
        av_opt_set_int(SwrCtx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    }else{
        fprintf(stderr, "Format error\n");
        return 1;
    }

    av_opt_set_int(SwrCtx, "out_sample_rate", Format->SampleRate, 0);

    if(Format->Encoding == AudioEncodingPCMS16LE){
        av_opt_set_sample_fmt(SwrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    }else if(Format->Encoding == AudioEncodingPCMS32LE){
        av_opt_set_sample_fmt(SwrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S32, 0);
    }else{
        fprintf(stderr, "Format error\n");
        return 1;
    }

    if(swr_init(SwrCtx)){
        fprintf(stderr, "Could not initialize software resampler\n");
        return 1;
    } 


    size64_t MaxSamplesInBuffer = Format->SampleRate;
    size64_t BufferSize = Format->NumberOfChannels * SampleSize * MaxSamplesInBuffer;
    void* Buffers[2]{ malloc(BufferSize), malloc(BufferSize)};
    size64_t SamplesWritten = 0;
    size64_t SizeWritten = 0;
    uint8_t BufferCount = 0;

    Frame = av_frame_alloc();

    int64_t LastIndex = 0;


    while(av_read_frame(FormatCtx, Packet) >= 0){
        if(avcodec_send_packet(CodecCtx, Packet)){
            fprintf(stderr, "Could not send packet for decoding\n");
            continue;
        }

        int Ret = 0;

        while(Ret >= 0){
            Ret = avcodec_receive_frame(CodecCtx, Frame);
            if(Ret == AVERROR_EOF || Ret == AVERROR(EAGAIN)){
                break;
            }else if(Ret){
                fprintf(stderr, "Could not decode frame %zd\n", Ret);
                break;
            }

            int SamplesToWrite = av_rescale_rnd(swr_get_delay(SwrCtx, CodecCtx->sample_rate) + Frame->nb_samples, Format->SampleRate, CodecCtx->sample_rate, AV_ROUND_UP);
            if(SamplesWritten + SamplesToWrite > MaxSamplesInBuffer){
                LastIndex = St->AddBuffer(Buffers[BufferCount], SizeWritten, LastIndex, FreeStreamBuffer);
                BufferCount = (BufferCount + 1) % 2;
                while(!IsBufferFree[BufferCount]){
                    asm("pause");
                }
                IsBufferFree[BufferCount] = false;
                SamplesWritten = 0;
                SizeWritten = 0;
            }
            uint8_t* Buffer = (uint8_t*)((uintptr_t)Buffers[BufferCount] + SizeWritten);
            int SamplesWrite = swr_convert(SwrCtx, &Buffer, (int)MaxSamplesInBuffer - SamplesWritten, (const uint8_t**)Frame->extended_data, Frame->nb_samples);

            if (SamplesWrite < 0) {
                printf("Resampling failed\n");
                //return 1;
            }

            SamplesWritten += SamplesWrite;
            SizeWritten += Format->NumberOfChannels * SampleSize * SamplesWrite;

            av_frame_unref(Frame);
            // if(SizeWritten > 0x100000){
            //     printf("%d:%d/%d:%d\n", TimestampSeconds / 60, TimestampSeconds % 60, DurationSeconds / 60, DurationSeconds % 60);
            //     St->AddBuffer(MusicBuffer, SizeWritten);
            //     return 0;
            // }
        }
        av_packet_unref(Packet);
    }
    return 0;
}