#ifndef _SRV_HID_H
#define _SRV_HID_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

event_t GetMouseRelativeEvent();
event_t GetMouseAbsoluteEvent();
event_t GetKeyboardServerEvent();
event_t GetKeyboardClientEvent();

KResult BindMouseRelative(thread_t Task, bool IgnoreMissedEvents);
KResult BindMouseAbsolute(thread_t Task, bool IgnoreMissedEvents);
KResult BindKeyboardClient(thread_t Task, bool IgnoreMissedEvents);

#if defined(__cplusplus)
}
#endif


#endif