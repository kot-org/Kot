#pragma once

#include <kot/sys.h>

typedef struct
{
    uint64_t fb_addr;
    bool visibility;
    uint32_t width;
    uint32_t height;
} WindowInfo;

extern WindowInfo *windowInfo;

void CreateWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

void ClearScreen(void);