#include <out/out.h>

/// @brief This function mix audio together
/// @tparam T 
/// @param Size in byte
/// @param Dst 
/// @param Src 
template <typename T> 
void MixAudio(size64_t Size, uint64_t Offset, audio_buffer_t* Dst, std::vector<audio_buffer_t> Src){
    // TODO mix
    assert(Size <= Dst->Buffer.Size);

    size64_t SrcCount = Src.size();
    if(Src.size() >= 1){
        size64_t SizeToCopy = Size;
        size64_t SizeSecondCopy = 0;
        if(Offset + SizeToCopy > Dst->Buffer.Size){
            SizeToCopy = Dst->Buffer.Size - Offset;
            if(Size > SizeToCopy){
                SizeSecondCopy = Size - SizeToCopy;
            }
        }
        memcpy((uintptr_t)((uint64_t)Dst->Buffer.Base + Offset), (uintptr_t)((uint64_t)Src[0].Buffer.Base + Offset), SizeToCopy);
        if(SizeSecondCopy){
            memcpy((uintptr_t)((uint64_t)Dst->Buffer.Base), (uintptr_t)((uint64_t)Src[0].Buffer.Base), SizeSecondCopy);
        }
    }
}

Outputs::Outputs(){
    Devices.push(NULL); // defautl output
    Lock = NULL;
}

uintptr_t FileBuf;
size64_t FileSize;
uint64_t FileOffset;

void OnOffsetUpdate(){
    OutputDevice_t* OutputDevice = (OutputDevice_t*)Sys_GetExternalDataThread();
    uint64_t Offset = (*(uint64_t*)((uint64_t)OutputDevice->OutputStream.Buffer.Base + OutputDevice->Device.PositionOfStreamData) + 0x1000) % OutputDevice->Device.StreamSize;
    size64_t SizeToProcess = OutputDevice->Device.SizeOffsetUpdateToTrigger + 0x1000;
    size64_t SizeToCopy = SizeToProcess;
    size64_t SizeSecondCopy = 0;
    if(Offset + SizeToCopy > OutputDevice->OutputStream.Buffer.Size){
        SizeToCopy = OutputDevice->OutputStream.Buffer.Size - Offset;
        if(SizeToProcess > SizeToCopy){
            SizeSecondCopy = SizeToProcess - SizeToCopy;
        }
    }
    memcpy((uintptr_t)((uint64_t)OutputDevice->InputStreams[0].Buffer.Base + Offset), (uintptr_t)((uint64_t)FileBuf + (FileOffset % FileSize)), SizeToCopy);
    if(SizeSecondCopy){
        memcpy((uintptr_t)((uint64_t)OutputDevice->InputStreams[0].Buffer.Base), (uintptr_t)((uint64_t)FileBuf + ((FileOffset + SizeToCopy) % FileSize)), SizeSecondCopy);
    }

    FileOffset += OutputDevice->Device.SizeOffsetUpdateToTrigger;


    if(OutputDevice->OutputStream.Format.Encoding == AudioEncodingPCMS8LE || OutputDevice->OutputStream.Format.Encoding == AudioEncodingPCMS16LE){
        MixAudio<uint16_t>(SizeToProcess, Offset, &OutputDevice->OutputStream, OutputDevice->InputStreams);
    }else{
        MixAudio<uint32_t>(SizeToProcess, Offset, &OutputDevice->OutputStream, OutputDevice->InputStreams);
    }
    Sys_Event_Close();
}

KResult Outputs::AddOutputDevice(srv_audio_device_t* Device){
    atomicAcquire(&Lock, 0);

    OutputDevice_t* OutputDevice = new OutputDevice_t;

    memcpy(&OutputDevice->Device, Device, sizeof(srv_audio_device_t));

    // Intialize stream buffer
    OutputDevice->OutputStream.Buffer.Base = GetFreeAlignedSpace(Device->StreamRealSize);
    Sys_AcceptMemoryField(Sys_GetProcess(), Device->StreamBufferKey, &OutputDevice->OutputStream.Buffer.Base);
    OutputDevice->OutputStream.Buffer.Size = Device->StreamSize;

    OutputDevice->OutputStream.Format = Device->Format;

    // Intialize stream events
    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&OnOffsetUpdate, PriviledgeApp, (uint64_t)OutputDevice, &OutputDevice->OnOffsetUpdateHandler);
    Sys_Event_Bind(Device->OnOffsetUpdate, OutputDevice->OnOffsetUpdateHandler, false);

    // Intialize stream status
    OutputDevice->OutputStream.Volume = 255;
    OutputDevice->StreamIsRunning = true;
    SetVolume(Device, OutputDevice->OutputStream.Volume);

    Devices.push(OutputDevice);

    // Set default
    if(!Devices[0]){
        Devices[0] = OutputDevice;
    }

    audio_buffer_t Buf{
        .Buffer{
            .Base = calloc(OutputDevice->OutputStream.Buffer.Size),
            .Size = OutputDevice->OutputStream.Buffer.Size,
        },
        .Format = OutputDevice->OutputStream.Format,
        .Volume = 255,
    };

    file_t* MusicFile = fopen("d1:Programs/Services/Audio/music.bin", "r");

    fseek(MusicFile, 0, SEEK_END);
    FileSize = ftell(MusicFile);
    fseek(MusicFile, 0, SEEK_SET);

    FileBuf = malloc(FileSize);
    fread(FileBuf, FileSize, 1, MusicFile);
    FileOffset = 0;

    OutputDevice->InputStreams.push(Buf);
    SetRunningState(Device, OutputDevice->StreamIsRunning);

    atomicUnlock(&Lock, 0);
    return KSUCCESS;
}