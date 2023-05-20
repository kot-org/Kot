#include <kot/uisd/srvs/hid.h>
#include <abi-bits/seek-whence.h>
#include <stdlib.h>
#include <string.h>

namespace Kot{
    /* Get event functions */

    kot_event_t GetMouseRelativeEvent(){
        uisd_hid_t* ControllerData = (uisd_hid_t*)FindControllerUISD(ControllerTypeEnum_Hid);
        return ControllerData->MouseRelative;
    }

    kot_event_t GetMouseAbsoluteEvent(){
        uisd_hid_t* ControllerData = (uisd_hid_t*)FindControllerUISD(ControllerTypeEnum_Hid);
        return ControllerData->MouseAbsolute;
    }

    kot_event_t GetKeyboardEvent(){
        uisd_hid_t* ControllerData = (uisd_hid_t*)FindControllerUISD(ControllerTypeEnum_Hid);
        return ControllerData->KeyboardEvent;
    }

    /* Bind functions */

    KResult BindMouseRelative(kot_thread_t Task, bool IgnoreMissedEvents){
        Sys_Event_Bind(GetMouseRelativeEvent(), Task, IgnoreMissedEvents);
        return KSUCCESS;
    }

    KResult BindMouseAbsolute(kot_thread_t Task, bool IgnoreMissedEvents){
        Sys_Event_Bind(GetMouseAbsoluteEvent(), Task, IgnoreMissedEvents);
        return KSUCCESS;
    }

    KResult BindKeyboardEvent(kot_thread_t Task, bool IgnoreMissedEvents){
        Sys_Event_Bind(GetKeyboardEvent(), Task, IgnoreMissedEvents);
        return KSUCCESS;
    }

    KResult GetTableConverter(char* Path, uintptr_t* TableConverter, size64_t* TableConverterCharCount){
        file_t* File = fopen(Path, "rb");
        if(!File){
            return KFAIL;
        }
        fseek(File, 0, SEEK_END);
        size64_t Size = ftell(File);
        fseek(File, 0, SEEK_SET);
        *TableConverter = (uintptr_t)malloc(Size);
        fread(*TableConverter, Size, 1, File);

        *TableConverterCharCount = Size / sizeof(char);

        fclose(File);
        return KSUCCESS;
    }

    KResult GetCharFromScanCode(uint64_t ScanCode, uintptr_t TableConverter, size64_t TableConverterCharCount, char* Char, bool* IsPressed, uint64_t* PressedCache){
        if(ScanCode > 0x80){
            *IsPressed = false;
            ScanCode -= 0x80;
        }else{
            *IsPressed = true;
        }
        
        if(ScanCode > TableConverterCharCount){
            return KFAIL;
        }

        char* LowerTableConverter = (char*)((uint64_t)TableConverter + sizeof(uint8_t));
        char* UpperTableConverter = (char*)((uint64_t)TableConverter + sizeof(uint8_t) + *(uint8_t*)TableConverter);

        if(LowerTableConverter[ScanCode] == 0xf){ // shift
            if(*IsPressed){
                *PressedCache = (1 << 0);
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
