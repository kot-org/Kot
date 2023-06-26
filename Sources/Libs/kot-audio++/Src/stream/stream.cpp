#include <kot-audio++/stream.h>

namespace Audio{
    
    void OnDeviceUpdateHandler(uint64_t DeviceID, uint64_t OldDeviceID){
        Stream* AudioStream = (Stream*)kot_Sys_GetExternalDataThread();
        AudioStream->OnDeviceUpdate(DeviceID, OldDeviceID);
        kot_Sys_Event_Close();
    }

    void OnOffsetUpdateHandler(){
        Stream* AudioStream = (Stream*)kot_Sys_GetExternalDataThread();
        AudioStream->OnOffsetUpdate();
        kot_Sys_Event_Close();        
    }

    Stream::Stream(uint64_t DeviceID){
        OutputID = DeviceID;

        /* Reset lock */
        Lock = NULL;

        /* Initialize offset update */
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&OnOffsetUpdateHandler, PriviledgeApp, (uint64_t)this, &OffsetUpdate);

        /* Initialize device update */
        kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&OnDeviceUpdateHandler, PriviledgeApp, (uint64_t)this, &DeviceUpdate);
        StreamBuffer = NULL;
        kot_Sys_Event_Bind(((kot_uisd_audio_t*)kot_FindControllerUISD(ControllerTypeEnum_Audio))->OnDeviceChanged, DeviceUpdate, false);

        /* Request stream, if event didn't do it before */
        atomicAcquire(&Lock, 0);
        if(!StreamBuffer){
            struct kot_srv_audio_callback_t* Callback = kot_Srv_Audio_RequestStream(DeviceID, true);
            StreamBuffer = (kot_audio_share_buffer_t*)Callback->Data;
            free(Callback);
            size64_t Size = 0;
            kot_Sys_GetInfoMemoryField(StreamBuffer->StreamBufferKey, NULL, &Size);
            LocalStreamBuffer.Base = kot_GetFreeAlignedSpace(Size);
            kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), StreamBuffer->StreamBufferKey, &LocalStreamBuffer.Base);
            LocalStreamBuffer.Size = StreamBuffer->StreamRealSize;
            kot_Sys_Event_Bind(StreamBuffer->OnOffsetUpdate, OffsetUpdate, false);
        }
        atomicUnlock(&Lock, 0);
    }

    Stream::~Stream(){
        kot_CloseStream(StreamBuffer);
        free(StreamBuffer);
    }

    kot_srv_audio_device_info_t* Stream::GetDeviceInfoStream(){
        kot_srv_audio_callback_t* Callback = kot_Srv_Audio_GetDeviceInfo(OutputID, true);
        if(Callback->Status != KSUCCESS){
            free(Callback);
            return NULL;
        }
        kot_srv_audio_device_info_t* Info = (kot_srv_audio_device_info_t*)Callback->Data;
        free(Callback);
        return Info;
    }

    KResult Stream::SetVolume(uint8_t Volume){
        return kot_ChangeVolumeStream(StreamBuffer, Volume);
    }

    KResult Stream::OnDeviceUpdate(uint64_t DeviceID, uint64_t OldDeviceID){
        if(DeviceID == OutputID){
            atomicAcquire(&Lock, 0);
            if(StreamBuffer){
                kot_Sys_CloseMemoryField(kot_Sys_GetProcess(), StreamBuffer->StreamBufferKey, LocalStreamBuffer.Base);
                free(StreamBuffer);
            }
            struct kot_srv_audio_callback_t* Callback = kot_Srv_Audio_RequestStream(OutputID, true);
            StreamBuffer = (kot_audio_share_buffer_t*)Callback->Data;   
            size64_t Size = 0;
            kot_Sys_GetInfoMemoryField(StreamBuffer->StreamBufferKey, NULL, &Size);
            LocalStreamBuffer.Base = kot_GetFreeAlignedSpace(Size);   
            kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), StreamBuffer->StreamBufferKey, &LocalStreamBuffer.Base);
            LocalStreamBuffer.Size = StreamBuffer->StreamRealSize; 
            kot_Sys_Event_Bind(StreamBuffer->OnOffsetUpdate, OffsetUpdate, false);     
            atomicUnlock(&Lock, 0);
        }
        return KSUCCESS;
    }

    KResult Stream::WriteBuffer(uint64_t Offset, size64_t SizeToProcess){
        ssize_t SizeToProcessFirst = SizeToProcess;
        ssize_t SizeToProcessEnd = 0;
        if(SizeToProcessFirst + Offset > StreamBuffer->StreamSize){
            SizeToProcessFirst = StreamBuffer->StreamSize - Offset;
            SizeToProcessEnd = SizeToProcess - SizeToProcessFirst;
        }
        
        memcpy((void*)((uintptr_t)LocalStreamBuffer.Base + Offset), (void*)((uintptr_t)InputStreams[0].Buffer.Base + InputStreams[0].Index), SizeToProcessFirst);
        InputStreams[0].Index += SizeToProcessFirst;
        memcpy(LocalStreamBuffer.Base, (void*)((uintptr_t)InputStreams[0].Buffer.Base + InputStreams[0].Index), SizeToProcessEnd);
        InputStreams[0].Index += SizeToProcessEnd;

        return KSUCCESS;
    }

    KResult Stream::LoadBuffer(){
        uint64_t Offset = (*(uint64_t*)((uint64_t)LocalStreamBuffer.Base + StreamBuffer->PositionOfStreamData) + (StreamBuffer->SizeOffsetUpdateToTrigger * 2)) % StreamBuffer->StreamSize;
        size64_t SizeToProcess = StreamBuffer->SizeOffsetUpdateToTrigger;
        if(InputStreams[0].Index + InputStreams[0].Buffer.Size > StreamBuffer->SizeOffsetUpdateToTrigger){
            SizeToProcess = InputStreams[0].Buffer.Size - InputStreams[0].Index;
        }
        WriteBuffer(Offset, SizeToProcess);
        // if(InputStreams[0].Index >= InputStreams[0].Buffer.Size){
        //     uint64_t NewOffset = Offset + SizeToProcess;
        //     size64_t NewSizeToProcess = StreamBuffer->SizeOffsetUpdateToTrigger - SizeToProcess;
        //     FindNext();
        //     if(InputStreams.size() >= 1){
        //         WriteBuffer(NewOffset, NewSizeToProcess);
        //     }
        // }
        return KSUCCESS;
    }

    KResult Stream::OnOffsetUpdate(){
        // TODO : mix more than one stream
        if(InputStreams.size() >= 1){
            if(InputStreams[0].Index >= InputStreams[0].Buffer.Size){
                FindNext();
                if(InputStreams.size() >= 1){
                    LoadBuffer();
                }
            }else{
                LoadBuffer();
            }
        }
        return KSUCCESS;
    }

    KResult Stream::FindNext(){
        atomicAcquire(&Lock, 1);
        KResult Status = KFAIL;
        if(InputStreams.size() >= 1){
            InputStreams[0].Callback(&InputStreams[0]);
            uint64_t NextIndex = InputStreams[0].NextIndex;
            if(NextIndex){
                InputStreams.set(0, InputStreams[NextIndex]);
                InputStreams.remove(NextIndex);
            }else{
                InputStreams.remove(0);
            }
            Status = KSUCCESS;
        }
        atomicUnlock(&Lock, 1);  
        return Status;     
    }

    int64_t Stream::AddBuffer(void* Base, size64_t Size, int64_t LastBufferIndex, BufferCallback Callback){
        atomicAcquire(&Lock, 1);
        Buffer* Last = NULL;

        if(LastBufferIndex >= 0 && LastBufferIndex < InputStreams.size()){
            Last = &InputStreams[LastBufferIndex];
        }

        Buffer NewBuffer{
            .Buffer{
                .Base = Base,
                .Size = Size,
            },
            .Index = 0,
            .NextIndex = 0,
            .Callback = Callback,
        };

        uint64_t Index = InputStreams.push(NewBuffer);
        if(Last){
            if(Last->Buffer.Base && Index != LastBufferIndex){
                Last->NextIndex = Index;
            }
        }
        atomicUnlock(&Lock, 1);
        return static_cast<int64_t>(Index);
    }

    kot_audio_format* Stream::GetStreamFormat(){
        return &StreamBuffer->Format;
    }

}