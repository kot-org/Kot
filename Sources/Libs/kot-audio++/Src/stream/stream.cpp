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
            kot_Sys_AcceptMemoryField(kot_Sys_GetProcess(), StreamBuffer->StreamBufferKey, &LocalStreamBuffer.Base);
            LocalStreamBuffer.Size = StreamBuffer->StreamRealSize; 
            kot_Sys_Event_Bind(StreamBuffer->OnOffsetUpdate, OffsetUpdate, false);     
            atomicUnlock(&Lock, 0);
        }
        return KSUCCESS;
    }

    template <typename T> 
    KResult Stream::MixAudioBuffers(){
        uint64_t Offset = (*(uint64_t*)((uint64_t)LocalStreamBuffer.Base + StreamBuffer->PositionOfStreamData) + (StreamBuffer->SizeOffsetUpdateToTrigger * 2)) % StreamBuffer->StreamSize;
        uint64_t SizeToProcess = StreamBuffer->SizeOffsetUpdateToTrigger;
        if(InputStreams[0].Index > InputStreams[0].Buffer.Size + StreamBuffer->SizeOffsetUpdateToTrigger){
            SizeToProcess = InputStreams[0].Index % StreamBuffer->SizeOffsetUpdateToTrigger;
        }
        for(uint64_t i = 0; i < SizeToProcess; i += sizeof(T)){
            *(T*)((uint64_t)LocalStreamBuffer.Base + ((Offset + i) % LocalStreamBuffer.Size)) = *(T*)((uint64_t)InputStreams[0].Buffer.Base + InputStreams[0].Index + i);
        }
        InputStreams[0].Index += SizeToProcess;

        return KSUCCESS;
    }

    KResult Stream::OnOffsetUpdate(){
        // TODO : mix more than one stream
        if(InputStreams.size() >= 1){
            if(InputStreams[0].Index >= InputStreams[0].Buffer.Size){
                InputStreams.remove(0);
                // Clear buffer
                memset(LocalStreamBuffer.Base, 0, LocalStreamBuffer.Size);
            }else{
                if(StreamBuffer->Format.Encoding == AudioEncodingPCMS8LE || StreamBuffer->Format.Encoding == AudioEncodingPCMS16LE){
                    MixAudioBuffers<uint16_t>();
                }else{
                    MixAudioBuffers<uint32_t>();
                }
            }
        }
        return KSUCCESS;
    }

    KResult Stream::AddBuffer(void* Base, size64_t Size){
        Buffer NewBuffer{
            .Buffer{
                .Base = Base,
                .Size = Size,
            },
            .Index = 0,
        };

        InputStreams.push(NewBuffer);

        return KSUCCESS;
    }

}