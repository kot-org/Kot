#include "boot.h"

void loadBootGraphics(){
    srv_system_callback_t* Callback = Srv_System_GetTableInRootSystemDescription("BGRT", true);
    if(Callback->Data != NULL){
        BGRTHeader_t* BGRTTable = (BGRTHeader_t*)Callback->Data;
        std::printf("ok");
    }
}