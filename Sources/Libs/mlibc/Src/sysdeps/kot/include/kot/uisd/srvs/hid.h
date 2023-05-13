#ifndef KOT_SRV_HID_H
#define KOT_SRV_HID_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/atomic.h>
#include <kot/memory.h>
#include <kot/uisd/srvs/storage.h>

namespace Kot{
    kot_event_t GetMouseRelativeEvent();
    kot_event_t GetMouseAbsoluteEvent();
    kot_event_t GetKeyboardEvent();

    KResult BindMouseRelative(kot_thread_t Task, bool IgnoreMissedEvents);
    KResult BindMouseAbsolute(kot_thread_t Task, bool IgnoreMissedEvents);
    KResult BindKeyboardEvent(kot_thread_t Task, bool IgnoreMissedEvents);

    KResult GetTableConverter(char* Path, uintptr_t* TableConverter, size64_t* TableConverterCharCount);
    KResult GetCharFromScanCode(uint64_t ScanCode, uintptr_t TableConverter, size64_t TableConverterCharCount, char* Char, bool* IsPressed, uint64_t* PressedCache);
}



#endif