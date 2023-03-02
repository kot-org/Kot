#pragma once

#include <kot/uisd.h>
#include <kot/utils.h>
#include <kot/types.h>

#include <kot-graphics/utils.h> 

#ifndef __KOT__GRAPHICS__ORB__
#define __KOT__GRAPHICS__ORB__

#if defined(__cplusplus)
extern "C" {
#endif

window_t* CreateWindow(event_t Event, uint64_t WindowType);
window_t* CloseWindow(window_t* Window);
KResult ResizeWindow(window_t* Window, int64_t Width, int64_t Height);
KResult WindowChangePosition(window_t* Window, uint64_t XPosition, uint64_t YPosition);
KResult ChangeVisibilityWindow(window_t* Window, bool IsHidden);

#if defined(__cplusplus)
}
#endif

#endif