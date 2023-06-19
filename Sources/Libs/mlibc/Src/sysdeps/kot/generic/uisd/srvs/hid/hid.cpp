#include <kot/uisd/srvs/hid.h>
#include <abi-bits/seek-whence.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern "C" {

/* Get event functions */

kot_event_t kot_GetMouseRelativeEvent(){
    kot_uisd_hid_t* ControllerData = (kot_uisd_hid_t*)kot_FindControllerUISD(ControllerTypeEnum_Hid);
    return ControllerData->MouseRelative;
}

kot_event_t kot_GetMouseAbsoluteEvent(){
    kot_uisd_hid_t* ControllerData = (kot_uisd_hid_t*)kot_FindControllerUISD(ControllerTypeEnum_Hid);
    return ControllerData->MouseAbsolute;
}

kot_event_t kot_GetKeyboardEvent(){
    kot_uisd_hid_t* ControllerData = (kot_uisd_hid_t*)kot_FindControllerUISD(ControllerTypeEnum_Hid);
    return ControllerData->KeyboardEvent;
}

/* Bind functions */

KResult kot_BindMouseRelative(kot_thread_t Task, bool IgnoreMissedEvents){
    kot_Sys_Event_Bind(kot_GetMouseRelativeEvent(), Task, IgnoreMissedEvents);
    return KSUCCESS;
}

KResult kot_BindMouseAbsolute(kot_thread_t Task, bool IgnoreMissedEvents){
    kot_Sys_Event_Bind(kot_GetMouseAbsoluteEvent(), Task, IgnoreMissedEvents);
    return KSUCCESS;
}

KResult kot_BindKeyboardEvent(kot_thread_t Task, bool IgnoreMissedEvents){
    kot_Sys_Event_Bind(kot_GetKeyboardEvent(), Task, IgnoreMissedEvents);
    return KSUCCESS;
}

KResult kot_GetTableConverter(char* Path, void** TableConverter, size64_t* TableConverterCharCount){
    kot_file_t* File = kot_fopen(Path, "rb");
    if(!File){
        return KFAIL;
    }
    kot_fseek(File, 0, SEEK_END);
    size64_t Size = kot_ftell(File);
    kot_fseek(File, 0, SEEK_SET);
    *TableConverter = (void*)malloc(Size);
    kot_fread(*TableConverter, Size, 1, File);

    *TableConverterCharCount = Size / sizeof(char);

    kot_fclose(File);
    return KSUCCESS;
}

KResult kot_GetCharFromScanCode(uint64_t ScanCode, void* TableConverter, size64_t TableConverterCharCount, char* Char, bool* IsPressed, uint64_t* PressedCache){
    if(ScanCode > 0x80){
        *IsPressed = false;
        ScanCode -= 0x80;
    }else{
        *IsPressed = true;
    }


    if(!Char) return KSUCCESS;
    
    if(ScanCode > TableConverterCharCount){
        return KFAIL;
    }

    char* LowerTableConverter = (char*)((uint64_t)TableConverter + sizeof(uint8_t));
    char* UpperTableConverter = (char*)((uint64_t)TableConverter + sizeof(uint8_t) + *(uint8_t*)TableConverter);

    if(LowerTableConverter[ScanCode] == 0xf){ // shift
        if(*IsPressed){
            *PressedCache |= (1 << 0);
        }else{
            *PressedCache &= ~(1 << 0);
        }
        return KBUSY;
    }

    if(*PressedCache & (1 << 0)){
        // convert to upper case
        *Char = UpperTableConverter[ScanCode];
    }else{
        *Char = LowerTableConverter[ScanCode];
    }

    return KSUCCESS;
}

}
