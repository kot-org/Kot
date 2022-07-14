#pragma once

#include <kot/sys.h>

void PutPixel(uint32_t x, uint32_t y, uint32_t color);

void DrawWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

void DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);