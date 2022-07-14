#pragma once

#include <kot/sys.h>

#include <kot/modules/wm/draw/draw.h>

WindowInfo_t* CreateWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void MoveWindow(WindowInfo_t* windowInfo, uint32_t x, uint32_t y);

void ClearScreen(void);