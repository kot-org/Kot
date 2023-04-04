#include <kot-audio++/stream.h>

namespace Audio{
    
    void OnDeviceUpdateHandler(uint64_t DeviceID, uint64_t OldDeviceID){
        Stream* AudioStream = (Stream*)Sys_GetExternalDataThread();
        AudioStream->OnDeviceUpdate(DeviceID, OldDeviceID);
        Sys_Event_Close();
    }

    void OnOffsetUpdateHandler(){
        Stream* AudioStream = (Stream*)Sys_GetExternalDataThread();
        AudioStream->OnOffsetUpdate();
        Sys_Event_Close();        
    }

    Stream::Stream(uint64_t DeviceID){
        OutputID = DeviceID;

        /* Reset lock */
        Lock = NULL;

        /* Initialize offset update */
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&OnOffsetUpdateHandler, PriviledgeApp, (uint64_t)this, &OffsetUpdate);

        /* Initialize device update */
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&OnDeviceUpdateHandler, PriviledgeApp, (uint64_t)this, &DeviceUpdate);
        StreamBuffer = NULL;

        /* Request stream, if event didn't do it before */
        atomicAcquire(&Lock, 0);
        if(!StreamBuffer){
            struct srv_audio_callback_t* Callback = Srv_Audio_RequestStream(DeviceID, true);
            StreamBuffer = (audio_share_buffer_t*)Callback->Data;
            free(Callback);
            Sys_AcceptMemoryField(Sys_GetProcess(), StreamBuffer->StreamBufferKey, &LocalStreamBuffer.Base);
            LocalStreamBuffer.Size = StreamBuffer->StreamRealSize;
            Sys_Event_Bind(StreamBuffer->OnOffsetUpdate, OffsetUpdate, false);
        }
        atomicUnlock(&Lock, 0);

    }

    Stream::~Stream(){
        CloseStream(StreamBuffer);
        free(StreamBuffer);
    }

    srv_audio_device_info_t* Stream::GetDeviceInfoStream(){
        srv_audio_callback_t* Callback = Srv_Audio_GetDeviceInfo(OutputID, true);
        if(Callback->Status != KSUCCESS){
            free(Callback);
            return NULL;
        }
        srv_audio_device_info_t* Info = (srv_audio_device_info_t*)Callback->Data;
        free(Callback);
        return Info;
    }

    KResult Stream::SetVolume(uint8_t Volume){
        return ChangeVolumeStream(StreamBuffer, Volume);
    }

    KResult Stream::OnDeviceUpdate(uint64_t DeviceID, uint64_t OldDeviceID){
        if(DeviceID == OutputID){
            atomicAcquire(&Lock, 0);
            if(StreamBuffer){
                Sys_CloseMemoryField(Sys_GetProcess(), StreamBuffer->StreamBufferKey, LocalStreamBuffer.Base);
                free(StreamBuffer);
            }
            struct srv_audio_callback_t* Callback = Srv_Audio_RequestStream(OutputID, true);
            StreamBuffer = (audio_share_buffer_t*)Callback->Data;      
            Sys_AcceptMemoryField(Sys_GetProcess(), StreamBuffer->StreamBufferKey, &LocalStreamBuffer.Base);
            LocalStreamBuffer.Size = StreamBuffer->StreamRealSize; 
            Sys_Event_Bind(StreamBuffer->OnOffsetUpdate, OffsetUpdate, false);     
            atomicUnlock(&Lock, 0);
        }
        return KSUCCESS;
    }

    KResult Stream::OnOffsetUpdate(){
        // TODO : mix more than one stream
        uint64_t Offset = (*(uint64_t*)((uint64_t)LocalStreamBuffer.Base + StreamBuffer->PositionOfStreamData) + (StreamBuffer->SizeOffsetUpdateToTrigger * 2)) % StreamBuffer->StreamSize;
        if(InputStreams.size() >= 1){
            memcpy((uintptr_t)((uint64_t)LocalStreamBuffer.Base + Offset), (uintptr_t)((uint64_t)InputStreams[0].Buffer.Base + InputStreams[0].Index), StreamBuffer->SizeOffsetUpdateToTrigger);
            InputStreams[0].Index += StreamBuffer->SizeOffsetUpdateToTrigger;
        }
        return KSUCCESS;
    }

    KResult Stream::AddBuffer(uintptr_t Base, size64_t Size){
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