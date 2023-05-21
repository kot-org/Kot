#ifndef KOT_SRV_HID_H
#define KOT_SRV_HID_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/atomic.h>
#include <kot/memory.h>
#include <kot/uisd/srvs/storage.h>

#if defined(__cplusplus)
extern "C" {
#endif

kot_event_t kot_GetMouseRelativeEvent();
kot_event_t kot_GetMouseAbsoluteEvent();
kot_event_t kot_GetKeyboardEvent();

KResult kot_BindMouseRelative(kot_thread_t Task, bool IgnoreMissedEvents);
KResult kot_BindMouseAbsolute(kot_thread_t Task, bool IgnoreMissedEvents);
KResult kot_BindKeyboardEvent(kot_thread_t Task, bool IgnoreMissedEvents);

KResult kot_GetTableConverter(char* Path, uintptr_t* TableConverter, size64_t* TableConverterCharCount);
KResult kot_GetCharFromScanCode(uint64_t ScanCode, uintptr_t TableConverter, size64_t TableConverterCharCount, char* Char, bool* IsPressed, uint64_t* PressedCache);



#if defined(__cplusplus)
} 
#endif

#endif