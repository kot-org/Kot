#include <audio/audio.h>

uint64_t LockAudio = 0;

const char* UnknowBuffer = "Unknow";
const char* EmptyBuffer = "";

void FreeStreamBufferEntry(void* St, Audio::Buffer* Buffer){
    AudioHandler* Audio = (AudioHandler*)((Audio::Stream*)St)->ExternalData;
    Audio->FreeStreamBuffer((Audio::Stream*)St, Buffer);
}

void DecodeAudioEntry(){
    AudioHandler* Audio = (AudioHandler*)kot_Sys_GetExternalDataThread();
    Audio->DecodeAudio();
    kot_Sys_Close(KSUCCESS);
}

AudioHandler::AudioHandler(){
    av_log_set_level(AV_LOG_QUIET);
    AvFrame = av_frame_alloc();
    AvPacket = av_packet_alloc();
    SwrCtx = swr_alloc();

    for(uint64_t i = 0; i < BUFFER_COUNT; i++){
        Buffers[i].Buffer = malloc(BUFFER_SIZE);
        Buffers[i].IsFree = true;
    }

    /* We use the first buffer */
    Buffers[0].IsFree = false;

    kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&DecodeAudioEntry, PriviledgeApp, (uint64_t)this, &DecodeAudioThread);

    TrackInfo.Metadata.Album = (char*)EmptyBuffer;
    TrackInfo.Metadata.Artist = (char*)EmptyBuffer;
    TrackInfo.Metadata.Title = (char*)EmptyBuffer;
    TrackInfo.Metadata.Description = (char*)EmptyBuffer;
}

AudioHandler::~AudioHandler(){

}


bool AudioHandler::GetPlay(){
    return IsPlaying; 
}

void AudioHandler::SetPlay(bool Status){
    if(Status != IsPlaying && TrackPath){
        IsPlaying = Status;
        kot_Sys_ExecThread(DecodeAudioThread, NULL, ExecutionTypeQueu, NULL);
    }else{
        IsPlaying = Status;
    }
}


void AudioHandler::SetCurrentTimestamp(float Time){
    if(!GetPlay()){
        atomicAcquire(&Lock, 1);
        if(!GetPlay()){
            Timestamp = Time;
        }else{
            TimestampSeek = Time;
            IsTimestampChanged = true;
        }
        atomicUnlock(&Lock, 1);
    }else{
        TimestampSeek = Time;
        IsTimestampChanged = true;
    }
}

float AudioHandler::GetCurrentTimestamp(){
    return Timestamp;
}


TrackInfo_t* AudioHandler::GetTrackInfo(){
    return &TrackInfo;
}


KResult AudioHandler::LoadTrack(char* Path){
    ClearTrack();
    atomicAcquire(&Lock, 0);
    AvFormatCtx = avformat_alloc_context();

    size_t PathLength = strlen(Path);
    TrackPath = (char*)malloc(PathLength + 1);
    strncpy(TrackPath, Path, PathLength);
    TrackPath[PathLength] = '\0';


    if(avformat_open_input(&AvFormatCtx, TrackPath, NULL, NULL)){
        avformat_free_context(AvFormatCtx);
        return KFAIL;
    }

    if(avformat_find_stream_info(AvFormatCtx, NULL)){
        avformat_free_context(AvFormatCtx);
        return KFAIL;
    }

    /* Get file metadata */
    AVDictionaryEntry* Tag = NULL;

    Tag = av_dict_get(AvFormatCtx->metadata, "album", Tag, AV_DICT_IGNORE_SUFFIX);
    if(Tag){
        TrackInfo.Metadata.Album = Tag->value;
    }else{
        TrackInfo.Metadata.Album = (char*)UnknowBuffer;
    }

    Tag = av_dict_get(AvFormatCtx->metadata, "artist", Tag, AV_DICT_IGNORE_SUFFIX);
    if(Tag){
        TrackInfo.Metadata.Artist = Tag->value;
    }else{
        TrackInfo.Metadata.Artist = (char*)UnknowBuffer;
    }

    Tag = av_dict_get(AvFormatCtx->metadata, "title", Tag, AV_DICT_IGNORE_SUFFIX);
    if(Tag){
        TrackInfo.Metadata.Title = Tag->value;
    }else{
        TrackInfo.Metadata.Title = (char*)UnknowBuffer;
    }

    Tag = av_dict_get(AvFormatCtx->metadata, "description", Tag, AV_DICT_IGNORE_SUFFIX);
    if(Tag){
        TrackInfo.Metadata.Description = Tag->value;
    }else{
        TrackInfo.Metadata.Description = (char*)UnknowBuffer;
    }
    
    /* Get audio duration */
    int64_t Duration = AvFormatCtx->duration + (AvFormatCtx->duration <= INT64_MAX - 5000 ? 5000 : 0);
    TrackInfo.Secs = Duration / AV_TIME_BASE;
    TrackInfo.Mins = TrackInfo.Secs / 60;
    TrackInfo.Secs %= 60;
    TrackInfo.Hours = TrackInfo.Mins / 60;
    TrackInfo.Mins %= 60;
    TrackInfo.Duration = (float)Duration / (float)AV_TIME_BASE;

    /* Initialize stream */
    AvStreamIndex = av_find_best_stream(AvFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    
    if(AvStreamIndex < 0){
        avformat_free_context(AvFormatCtx);
        return KFAIL;
    }

    AvStream = AvFormatCtx->streams[AvStreamIndex];

    /* initialize codec */
    const AVCodec* Codec = avcodec_find_decoder(AvStream->codecpar->codec_id);
    if (!Codec) {
        avformat_free_context(AvFormatCtx);
        return KFAIL;
    }

    AvCodecCtx = avcodec_alloc_context3(Codec);

    if(AvCodecCtx == NULL){
        avformat_free_context(AvFormatCtx);
        return KFAIL;
    }

    if(avcodec_parameters_to_context(AvCodecCtx, AvStream->codecpar)){
        avformat_free_context(AvFormatCtx);
        return KFAIL;
    }

    if(avcodec_open2(AvCodecCtx, Codec, NULL) < 0){
        avformat_free_context(AvFormatCtx);
        return KFAIL;
    }
    
    atomicUnlock(&Lock, 0);

    ReloadSwr();

    return KSUCCESS;
}

void AudioHandler::ClearTrack(){
    atomicAcquire(&Lock, 0);

    SetPlay(false);
    free(TrackPath);
    TrackPath = NULL;

    avformat_free_context(AvFormatCtx);

    atomicUnlock(&Lock, 0);
}

Audio::Stream* AudioHandler::GetStream(){
    return Stream;
}

KResult AudioHandler::SetStream(Audio::Stream* St){
    St->ExternalData = this;

    Stream = St;
    Format = St->GetStreamFormat();

    SampleSize = kot_Srv_Audio_GetEncodingSize(Format->Encoding);
    MaxSamplesInBuffer = (BUFFER_SIZE / Format->NumberOfChannels) / SampleSize;

    ReloadSwr();

    return KSUCCESS;
}

void AudioHandler::FreeStreamBuffer(Audio::Stream* St, Audio::Buffer* Buffer){
    atomicAcquire(&Lock, 2);
    Buffers[LastBufferFree].IsFree = true;
    LastBufferFree = (LastBufferFree + 1) % BufferCount;
    if(IsDecodePaused){
        IsDecodePaused = false;
        kot_Sys_Unpause(DecodeAudioThread);
    }
    atomicUnlock(&Lock, 2);
}

void AudioHandler::DecodeAudio(){
    atomicAcquire(&Lock, 1);
    while(av_read_frame(AvFormatCtx, AvPacket) >= 0 && GetPlay()){
        if(avcodec_send_packet(AvCodecCtx, AvPacket)){
            av_packet_unref(AvPacket);
            continue;
        }

        int Ret = 0;

        while(Ret >= 0){
            Ret = avcodec_receive_frame(AvCodecCtx, AvFrame);

            if(Ret == AVERROR_EOF || Ret == AVERROR(EAGAIN)){
                break;
            }else if(Ret){
                break;
            }

            ssize_t SamplesToWrite = av_rescale_rnd(swr_get_delay(SwrCtx, AvCodecCtx->sample_rate) + AvFrame->nb_samples, Format->SampleRate, AvCodecCtx->sample_rate, AV_ROUND_UP);

            if(SamplesWritten + SamplesToWrite > MaxSamplesInBuffer){
                LastIndexStreamBuffer = Stream->AddBuffer(Buffers[LastBufferAllocated].Buffer, SizeWritten, LastIndexStreamBuffer, FreeStreamBufferEntry);

                LastBufferAllocated = (LastBufferAllocated + 1) % BufferCount;
                if(!Buffers[LastBufferAllocated].IsFree){
                    atomicAcquire(&Lock, 2);
                    IsDecodePaused = true;
                    atomicUnlock(&Lock, 2);
                    kot_Sys_Pause(false);
                }

                Buffers[LastBufferAllocated].IsFree = false;
                SamplesWritten = 0;
                SizeWritten = 0;
            }
            uint8_t* Buffer = (uint8_t*)((uintptr_t)Buffers[LastBufferAllocated].Buffer + SizeWritten);
            ssize_t SamplesWrite = swr_convert(SwrCtx, &Buffer, (int)MaxSamplesInBuffer - SamplesWritten, (const uint8_t**)AvFrame->extended_data, AvFrame->nb_samples);
            if(SamplesWrite < 0){
                continue;
            }
            SamplesWritten += SamplesWrite;
            SizeWritten += Format->NumberOfChannels * SampleSize * SamplesWrite;

            Timestamp = (float)AvFrame->best_effort_timestamp / (float)AvStream->time_base.den;
            kot_Sys_Event_Trigger(OnNewFrameEvent, &OnNewFrameEventArgs);

            av_frame_unref(AvFrame);
        }
        av_packet_unref(AvPacket);

        if(IsTimestampChanged){
            // Flush last buffer
            SamplesWritten = 0;
            SizeWritten = 0;

            avcodec_flush_buffers(AvCodecCtx);
            swr_convert(SwrCtx, NULL, 0, NULL, 0);

            Timestamp = TimestampSeek;
            av_seek_frame(AvFormatCtx, AvStreamIndex, (long)(Timestamp / av_q2d(AvStream->time_base)), 0);
            IsTimestampChanged = false;
        }
    }
    atomicUnlock(&Lock, 1);
}

KResult AudioHandler::ReloadSwr(){
    if(!AvCodecCtx || !Format){
        return KFAIL;
    }

    atomicAcquire(&Lock, 1);

    swr_close(SwrCtx);

    if(AvCodecCtx->channels == 1){
        av_opt_set_int(SwrCtx, "in_channel_layout", AV_CH_LAYOUT_MONO, 0);
    }else if(Format->NumberOfChannels == 2){
        av_opt_set_int(SwrCtx, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    }else{
        return KFAIL;
    }

    av_opt_set_int(SwrCtx, "in_sample_rate", AvCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(SwrCtx, "in_sample_fmt", AvCodecCtx->sample_fmt, 0);

    if(Format->NumberOfChannels == 1){
        av_opt_set_int(SwrCtx, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
    }else if(Format->NumberOfChannels == 2){
        av_opt_set_int(SwrCtx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    }else{
        return KFAIL;
    }

    av_opt_set_int(SwrCtx, "out_sample_rate", Format->SampleRate, 0);

    if(Format->Encoding == AudioEncodingPCMS16LE){
        av_opt_set_sample_fmt(SwrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    }else if(Format->Encoding == AudioEncodingPCMS32LE){
        av_opt_set_sample_fmt(SwrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S32, 0);
    }else{
        return KFAIL;
    }

    if(swr_init(SwrCtx)){
        return KFAIL;
    }

    atomicUnlock(&Lock, 1);

    return KSUCCESS;
}
