#include <out/out.h>


KResult AddOutputDevice(srv_audio_device_t* Device){
    std::printf("%x", Device->Type);
    SetVolume(Device, 255);
    return KSUCCESS;
}